//
// Created by gsr on 2022/5/28.
//

#include "../include/http_parse.h"
#include "../include/logger.h"

int HttpRequestParser::get_line(int fd, char *buffer, int size){
    int count = 0;
    char ch = '\0';
    int len = 0;

    while( (count<size - 1) && ch != '\n'){
        len = recv(fd, &ch, 1, 0);      //一个字符一个字符接收

        if(len == 1){
            if(ch == '\r'){
                continue;
            } else if(ch == '\n'){
                //buf[count] = '\0';
                break;
            }

            //这里处理一般的字符
            buffer[count] = ch;
            count++;

        } else if( len == -1 ){//读取出错
//            perror("read failed");
            count = -1;
            break;
        } else {// read 返回0,客户端关闭sock 连接.
            fprintf(stderr, "client close.\n");
            count = -1;
            break;
        }
    }

    if(count >= 0) buffer[count] = '\0';

    return count;
}

HttpRequestParser::HTTP_CODE HttpRequestParser::parse_requestline(int client_fd, HttpRequest &request) {

    int len = 0;
    char buf[256];
    char method[64];
    char url[256];
    char http_version[64];

    //1.读取请求行
    len = get_line(client_fd, buf, sizeof(buf));

    if (len > 0) {//读到了请求行
        int i = 0, j = 0;
        while (!isspace(buf[j]) && (i < sizeof(method) - 1)) {
            method[i] = buf[j];
            i++;
            j++;
        }
        method[i] = '\0';
        while (isspace(buf[j++]));//跳过白空格
        --j;
        i = 0;
        while (!isspace(buf[j]) && (i < sizeof(url) - 1)) {
            url[i] = buf[j];
            i++;
            j++;
        }
        url[i] = '\0';
        while (isspace(buf[j++]));//跳过白空格
        --j;
        i = 0;
        while (!isspace(buf[j]) && buf[j]!='\n' && buf[j]!='\r' && (i < sizeof(http_version) - 1)) {
            http_version[i] = buf[j];
            i++;
            j++;
        }
        http_version[i] = '\0';

        if (strcasecmp(method, "GET") == 0) {   //匹配方法
            request.mMethod = HttpRequest::GET;
        } else if (strcasecmp(method, "POST") == 0) {
            request.mMethod = HttpRequest::POST;
        } else if (strcasecmp(method, "PUT") == 0) {
            request.mMethod = HttpRequest::PUT;
        } else if (strcasecmp(method, "DELETE") == 0) {
            request.mMethod = HttpRequest::DELETE;
        } else {
            return BAD_REQUEST;
        }

        if (strncasecmp("HTTP/1.1", http_version, 8) == 0) {     // 设定HTTP版本号
            request.mVersion = HttpRequest::HTTP_11;
        } else if (strncasecmp("HTTP/1.0", http_version, 8) == 0) {
            request.mVersion = HttpRequest::HTTP_10;
        } else {
            return BAD_REQUEST;
        }

        bool is_index = false;          //如果url为'/'，则去到默认初始的index.html页面
        char *url_ = url;
        if (strncasecmp(url_, "http://", 7) == 0) {      // 如果url是一个完整的url
            url_ += 7;
            url_ = strchr(url_, '/');     // 定位到uri起始处
            if ( !(url_+1) ){
                is_index = true;
            }
        } else if (strncasecmp(url_, "/", 1) == 0) {     // 如果url不完整，则直接查找是否存在第一个 /
            if (strlen(url) == 1 && url[0] == '/'){
                is_index = true;
            }
        } else {
            return BAD_REQUEST;
        }
        request.mUri = std::string(url);

        //定位url文件路径
        {
            char *pos = strchr(url, '?');
            if(pos){
                *pos = '\0';
            }
        }
        if(is_index)    sprintf(request.mRealPath, "../html_docs/index.html");
        else sprintf(request.mRealPath, "../html_docs%s", url);


        return NO_REQUEST;//继续解析
    } else{
//        std::cout << client_fd <<" recv had no comment\n";
        return BAD_REQUEST;
    }
}

HttpRequestParser::HTTP_CODE HttpRequestParser::parse_headers(int client_fd, HttpRequest &request) {


    char buf[256];
    int len;
    char header[64];
    char var[256];

    do{
        len = get_line(client_fd, buf, sizeof(buf));
        if (len <= 0) break;

        int i = 0, j = 0;
        while (!isspace(buf[j]) && buf[j] != ':' && (i < sizeof(header) - 1)) {
            header[i] = buf[j];
            i++;
            j++;
        }
        header[i] = '\0';
        ++j;
        while (isspace(buf[j++]));//跳过白空格
        --j;
        i = 0;
        while (!isspace(buf[j]) && (i < sizeof(var) - 1)) {
            var[i] = buf[j];
            i++;
            j++;
        }
        var[i] = '\0';

        HttpRequest::HTTP_HEADER tmpheader;
        if (strcasecmp(header, "Host") == 0) {   //匹配方法
            tmpheader = HttpRequest::Host;
        } else if (strcasecmp(header, "User_Agent") == 0) {
            tmpheader = HttpRequest::User_Agent;
        } else if (strcasecmp(header, "Connection") == 0) {
            tmpheader = HttpRequest::Connection;
        } else if (strcasecmp(header, "Accept_Encoding") == 0) {
            tmpheader = HttpRequest::Accept_Encoding;
        } else if (strcasecmp(header, "Accept_Language") == 0){
            tmpheader = HttpRequest::Accept_Language;
        } else if (strcasecmp(header, "Accept") == 0){
            tmpheader = HttpRequest::Accept;
        } else if (strcasecmp(header, "Cache_Control") == 0){
            tmpheader = HttpRequest::Cache_Control;
        } else if (strcasecmp(header, "Upgrade_Insecure_Requests") == 0) {
            tmpheader = HttpRequest::Upgrade_Insecure_Requests;
        }else {
            continue;
        }

        request.mHeaders.insert(std::pair<HttpRequest::HTTP_HEADER, std::string>(tmpheader, std::string(var)));

    } while(len>0);
    if (request.mMethod == HttpRequest::GET) {  // 如果是GET方法，此处解析终止，解析器返回GET_REQUEST表示已收到请求
        return FINISH_REQUEST;
    }

    return NO_REQUEST;//继续解析
}

HttpRequestParser::HTTP_CODE HttpRequestParser::parse_body(int client_fd, HttpRequest &request) {
    read(client_fd, &request.mContent, 1024);
    return FINISH_REQUEST;
}

HttpRequestParser::HTTP_CODE HttpRequestParser::parse_content(int client_fd, HttpRequest &request){
    if (HttpRequestParser::parse_requestline(client_fd, request) == BAD_REQUEST){
        LogFile::writeInfo("parse_requestline BAD_REQUEST\n");
//        std::cout << "parse_requestline BAD_REQUEST\n";
        return BAD_REQUEST;
    }
    if (HttpRequestParser::parse_headers(client_fd, request) == BAD_REQUEST){
        LogFile::writeInfo("parse_headers BAD_REQUEST\n");
//        std::cout << "parse_headers BAD_REQUEST\n";
        return BAD_REQUEST;
    } else if (HttpRequestParser::parse_headers(client_fd, request) == FINISH_REQUEST){
        return FINISH_REQUEST;
    }
    HttpRequestParser::parse_body(client_fd, request);
    return FINISH_REQUEST;
}