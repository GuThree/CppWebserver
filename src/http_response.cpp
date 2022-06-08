//
// Created by gsr on 2022/5/29.
//

#include "../include/http_response.h"

const char * not_found = "\
                                <html lang=\"zh-CN\">\n\
                                    <head>\n\
                                    <meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\">\n\
                                        <title>NOT FOUND</title>\n\
                                    </head>\n\
                                    <body>\n\
                                        <div align=center height=\"500px\" >\n\
                                            <h1>404</h1>\
                                            <h2>The server could not fulfill your request because the resource specified is unavailable or nonexistent.</h2>\n\
                                        </div>\n\
                                    </body>\n\
                                </html>";


const char * internal_fault = "\
                                <html lang=\"zh-CN\">\r\n\
                                    <meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\">\r\n\
                                    <head>\r\n\
                                        <title>Inner Error</title>\r\n\
                                    </head>\r\n\
                                    <body>\r\n\
                                        <P>服务器内部出错.\r\n\
                                    </body>\r\n\
                                </html>";

HttpResponse::HttpResponse() : mStatusCode(Unknow), keep_alive(true), mStatusMsg(""), mSuffix(""), mFilePath(nullptr), mContentLength(0),
                               mBody(nullptr), mVersion(HttpRequest::HTTP_11) {};

void HttpResponse::appendBuffer(char *buffer) const{

    // 版本
    if (mVersion == HttpRequest::HTTP_11) {
        sprintf(buffer, "HTTP/1.1 %d %s\r\n", mStatusCode, mStatusMsg.c_str());
    } else {
        sprintf(buffer, "HTTP/1.0 %d %s\r\n", mStatusCode, mStatusMsg.c_str());
    }

    // 头部字段
    for (auto it = mHeaders.begin(); it != mHeaders.end(); it++) {
        sprintf(buffer, "%s%s: %s\r\n", buffer, it->first.c_str(), it->second.c_str());
    }
    sprintf(buffer, "%sContent-type: %s\r\n", buffer, suffix_map.find(mSuffix)->second.c_str());
    // keep_alive
    if (keep_alive) {
        sprintf(buffer, "%sConnection: keep-alive\r\n", buffer);
    } else {
        sprintf(buffer, "%sConnection: close\r\n", buffer);
    }
    //body
    sprintf(buffer, "%s\r\n", buffer);
    sprintf(buffer, "%s%s", buffer, this->mBody);

}



void HttpResponse::analysis(int status, HttpRequest &request){

    //先判断请求报文是否正常解析
    if (status == 0){
        this->mStatusCode = _200Ok;
    } else {    //服务器不正常解析 500状态码
        this->mStatusCode = _500InternalFault;
        this->mVersion = request.mVersion;
        this->mStatusMsg = "Internal Sever Error";
        this->mSuffix = ".html";
        this->mBody = const_cast<char *>(internal_fault);
        this->mContentLength = strlen(this->mBody);
        addHeader("Content-Length", std::to_string(this->mContentLength));
        appendBuffer(this->response_message);
        return;
    }

    struct stat st;
    //判断文件是否存在，如果存在就响应200 OK,如果不存在，就响应 404 NOT FOUND.
    if(stat(request.mRealPath, &st) == -1){//文件不存在或是出错
        this->mStatusCode = _404NotFound;
        this->mVersion = request.mVersion;
        this->mStatusMsg = "NOT FOUND";
        this->mSuffix = ".html";
        this->mBody = const_cast<char *>(not_found);
        this->mContentLength = strlen(this->mBody);
        addHeader("Content-Length", std::to_string(this->mContentLength));
        appendBuffer(this->response_message);
        return;
    } else {     //文件存在
        this->mStatusCode = _200Ok;
        if(S_ISDIR(st.st_mode)){    //是文件夹
            if ( request.mRealPath[strlen(request.mRealPath) - 1] != '/') {     //判断路径最后一个字符是不是'/'
                request.mRealPath[strlen(request.mRealPath)] = '/';
                request.mRealPath[strlen(request.mRealPath) + 1] = '\0';
            }
            strcat(request.mRealPath, "index.html");   //要求每个文件夹有默认的index.html文件
            this->mFilePath = request.mRealPath;
        }
        FILE *resource = NULL;
        resource = fopen(request.mRealPath, "r");
        if(resource == NULL) {
            std::cout << "file opens error" << std::endl;
        }

        this->mVersion = request.mVersion;
        this->mStatusMsg = "OK";
        this->mSuffix = strrchr(request.mRealPath, '.');

        char buf[5120];
        char str[128];
        while(fgets(str, 128, resource) != NULL){
            sprintf(buf, "%s%s", buf, str);
        }
        this->mBody = buf;
        this->mContentLength = strlen(buf);
        addHeader("Server", "Gusirui Server");
        addHeader("Content-Length", std::to_string(this->mContentLength));
        appendBuffer(this->response_message);
    }
}

void HttpResponse::showInfo() {
    std::cout << "--------------------Response Information--------------------" << std::endl;
    std::cout << "Version:" << this->mVersion << std::endl;
    std::cout << "Status Code:" << this->mStatusCode << " " << this->mStatusMsg << std::endl;
    for (auto it = this->mHeaders.begin(); it != this->mHeaders.end(); it++) {
        std::cout << it->first << ":" << it->second << std::endl;
    }
    std::cout << this->mBody << std::endl;
    std::cout << "-------------------------------------------------------------" << std::endl;

}