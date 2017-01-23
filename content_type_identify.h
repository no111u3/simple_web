//
// Created by nis on 21.01.17.
//

#ifndef CONTENT_TYPE_IDENTIFY_H
#define CONTENT_TYPE_IDENTIFY_H

#include <map>
#include <string>

namespace http {
    static std::map<const std::string, const char *> content_types = {
            {"html", "text/html"},
            {"css", "text/css"},
            {"png", "image/png"},
            {"jpeg", "image/jpeg"},
            {"js", "text/javascript"}
    };
}

#endif //CONTENT_TYPE_IDENTIFY_H
