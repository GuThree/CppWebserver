//
// Created by ASUS on 2022/6/18.
//

#include <iostream>
#include "../include/logger.h"

FILE * LogFile::logfile = fopen("../files/log.txt", "w");

void LogFile::writeInfo(const char *info) {
    fwrite(info, sizeof(char) , strlen(info), logfile);
}

void LogFile::closeFile() {
    fclose(logfile);
}