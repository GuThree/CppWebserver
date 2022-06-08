cmake_minimum_required(VERSION 2.8)
project(cwebserver)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

add_executable(cwebserver src/main.cpp
        src/lock.cpp include/lock.h
        src/threadpool.cpp include/threadpool.h
        src/socket.cpp include/socket.h
        src/http_request.cpp include/http_request.h
        src/http_parse.cpp include/http_parse.h
        src/http_response.cpp include/http_response.h
        src/http_data.cpp include/http_data.h
        src/timer.cpp include/timer.h
        src/epoll.cpp include/epoll.h
        src/server.cpp include/server.h)