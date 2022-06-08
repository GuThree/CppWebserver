//
// Created by gsr on 2022/5/29.
//

//#ifndef CWEBSERVER_HTTP_REQUEST_H
//#define CWEBSERVER_HTTP_REQUEST_H

#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

class HttpRequest;

struct HttpRequest {


    enum HTTP_VERSION {     //版本枚举集合
        HTTP_10 = 0, HTTP_11, VERSION_NOT_SUPPORT
    };

    enum HTTP_METHOD {      //方法枚举集合，目前DELETE尚未实现
        GET = 0, POST, PUT, DELETE, METHOD_NOT_SUPPORT
    };

    enum HTTP_HEADER {      //头部字段枚举集合
        Host = 0,
        User_Agent,
        Connection,
        Accept_Encoding,
        Accept_Language,
        Accept,
        Cache_Control,
        Upgrade_Insecure_Requests
    };

    struct EnumClassHash {      //散列算法的实现，用来对HTTP_HEADER进行处理
        template<typename T>
        std::size_t operator()(T t) const {
            return static_cast<std::size_t>(t);     //把对应值强行解释为size_t类型作为哈希值
        }
    };


    HttpRequest(std::string url = std::string(""), HTTP_METHOD method = METHOD_NOT_SUPPORT, HTTP_VERSION version = VERSION_NOT_SUPPORT);

    void showInfo();

    //请求行字段
    HTTP_METHOD mMethod;
    HTTP_VERSION mVersion;
    std::string mUri;
    char mRealPath[512];    //去到根目录下的文件路径
    //请求体
    char *mContent;
    //请求头
    std::unordered_map<HTTP_HEADER, std::string, EnumClassHash> mHeaders;

};


//#endif //CWEBSERVER_HTTP_REQUEST_H
