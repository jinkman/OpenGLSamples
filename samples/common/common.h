#ifndef COMMON_H
#define COMMON_H

#include <Config.h>
#include <string>

std::string getLocalPath(const std::string &path) {
    return std::string(DATA_PATH) + path;
}

#endif