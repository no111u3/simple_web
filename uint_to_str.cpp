//
// Created by nis on 19.01.17.
//

#include "uint_to_str.h"

namespace util {
    size_t const get_length(uint32_t value) {
        if (value < 1e5) {
            if (value < 1e3) {
                if (value < 1e2) {
                    if (value < 1e1) {
                        return 1;
                    } else {
                        return 2;
                    }
                } else {
                    return 3;
                }
            } else {
                if (value < 1e4) {
                    return 4;
                } else {
                    return 5;
                }
            }
        } else {
            if (value < 1e7) {
                if (value < 1e6) {
                    return 6;
                } else {
                    return 7;
                }
            } else {
                if (value < 1e9) {
                    if (value < 1e8) {
                        return 8;
                    } else {
                        return 9;
                    }
                } else {
                    return 10;
                }
            }
        }
    }

    size_t uint32_to_str(uint32_t value, char *dst) {
        static const char digits[201] =
                "0001020304050607080910111213141516171819"
                "2021222324252627282930313233343536373839"
                "4041424344454647484950515253545556575859"
                "6061626364656667686970717273747576777879"
                "8081828384858687888990919293949596979899";
        size_t const length = get_length(value);
        size_t next = length - 1;
        while (value >= 100) {
            auto const i = (value % 100) * 2;
            value /= 100;
            dst[next] = digits[i + 1];
            dst[next - 1] = digits[i];
            next -= 2;
        }
        // Handle last 1-2 digits
        if (value < 10) {
            dst[next] = (char) ('0' + uint32_t(value));
        } else {
            auto i = uint32_t(value) * 2;
            dst[next] = digits[i + 1];
            dst[next - 1] = digits[i];
        }
        return length;
    }
    }

