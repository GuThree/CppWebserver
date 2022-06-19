//
// Created by gsr on 2022/6/1.
//

//#ifndef CWEBSERVER_LOGGER_H
//#define CWEBSERVER_LOGGER_H
#pragma once

#include <cstring>
#include <string>

class LogFile{
public:

    static void writeInfo(const char *info);
    static void closeFile();

public:
    static FILE *logfile;
};

//#endif //CWEBSERVER_LOGGER_H
