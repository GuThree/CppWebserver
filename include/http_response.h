//
// Created by gsr on 2022/5/29.
//

//#ifndef CWEBSERVER_HTTP_RESPONSE_H
//#define CWEBSERVER_HTTP_RESPONSE_H

#pragma once

#include "http_request.h"
#include <sys/stat.h>
#include <unordered_map>
#include <cstring>

class HttpResponse{

public:
    std::unordered_map<std::string, std::string> suffix_map = {
            {".html", "text/html"},
            {".xml", "text/xml"},
            {".xhtml", "application/xhtml+xml"},
            {".txt", "text/plain"},
            {".rtf", "application/rtf"},
            {".pdf", "application/pdf"},
            {".word", "application/msword"},
            {".png", "image/png"},
            {".gif", "image/gif"},
            {".jpg", "image/jpeg"},
            {".jpeg", "image/jpeg"},
            {".au", "audio/basic"},
            {".mpeg", "video/mpeg"},
            {".mpg", "video/mpeg"},
            {".avi", "video/x-msvideo"},
            {".gz", "application/x-gzip"},
            {".tar", "application/x-tar"},
            {".css", "text/css"},
            {"", "text/plain"},
            {"default","text/plain"}
    };


    enum HttpStatusCode {   // HTTP状态码枚举集合
        Unknow,
        _200Ok = 200,
        _404NotFound = 404,
        _500InternalFault = 500
    };

    HttpResponse();


    void appendBuffer(char *) const;
    void analysis(int status ,HttpRequest &request);
    void showInfo();

    bool isKeepAlive(){
        return keep_alive;
    }

    void setKeepAlive(bool keepAlive) {
        keep_alive = keepAlive;
    }

    void addHeader(const std::string &key, const std::string &value) {
        mHeaders[key] = value;
    }

    char response_message[10240];
private:
    HttpRequest::HTTP_VERSION mVersion;     // 响应的HTTP版本
    HttpStatusCode mStatusCode;     // 响应状态码
    std::string mStatusMsg;     // 状态解释性信息
    std::string mSuffix;     //后缀信息
    char *mBody;      // 返回体中的body
    const char *mFilePath;  // 文件路径
    int mContentLength;     // 包含的内容的长度
    bool keep_alive;    // 是否保持链接
    std::unordered_map<std::string, std::string> mHeaders;  //  头部字段
};

//#endif //CWEBSERVER_HTTP_RESPONSE_H
