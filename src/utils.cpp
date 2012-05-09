#include "utils.h"

std::string getBinPath(const char* bin) {
    std::string tmp = bin;

    std::size_t pos;

    if (tmp.find("/") == std::string::npos) {
        // assume winders strings
        pos = tmp.find_last_of("\\");
    } else {
        pos = tmp.find_last_of("/");
    }
    pos++;

    return tmp.substr(0, pos);
}
