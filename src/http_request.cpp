//
// Created by gsr on 2022/5/29.
//

#include "../include/http_request.h"

HttpRequest::HttpRequest(std::string url, HttpRequest::HTTP_METHOD method, HttpRequest::HTTP_VERSION version) :
        mMethod(method), mVersion(version), mUri(url), mContent(nullptr), mHeaders(std::unordered_map<HTTP_HEADER, std::string, EnumClassHash>()) {
}

void HttpRequest::showInfo(){
    std::cout << "--------------------Request Information--------------------" << std::endl;
    std::cout << "real_path:" << this->mRealPath << std::endl;
    std::cout << "method:" << this->mMethod << std::endl;
    std::cout << "url:" << this->mUri << std::endl;
    std::cout << "version:" << this->mVersion << std::endl;
    for (auto it = this->mHeaders.begin(); it != this->mHeaders.end(); it++) {
        std::cout << it->first << ":" << it->second << std::endl;
    }
    if ( this->mContent != nullptr )
        std::cout << this->mContent << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
}