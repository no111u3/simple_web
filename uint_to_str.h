//
// Created by nis on 19.01.17.
//

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <cstdio>

namespace util {
    size_t const get_length(uint32_t value);

    size_t uint32_to_str(uint32_t value, char *dst);
}

#endif //PROJECT_UTILS_H
