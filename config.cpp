//
// Created by nis on 25.12.16.
//
#include <iostream>
#include <getopt.h>
#include "config.h"

namespace conf {

    Config get_config(int argc, char **argv) {
        Config config;
        config.address.sin_family = PF_INET;
        config.address.sin_port = htons(12345);
        config.address.sin_addr.s_addr = inet_addr("127.0.0.1");

        config.not_daemon = false;
        config.once = false;

        int res;
        while ((res = getopt(argc, argv, "h:p:d:ns")) != -1) {
            switch (res) {
                case 'h':
                    config.address.sin_addr.s_addr = inet_addr(optarg);
                    break;
                case 'p':
                    config.address.sin_port = htons((unsigned short )std::stoi(optarg));
                    break;
                case 'd':
                    config.directory = optarg;
                    break;
                case 'n':
                    config.not_daemon = true;
                    break;
                case 's':
                    config.once = true;
                    break;
                default:
                    std::cout << "Unknown argument: " << (char)res << std::endl;
                    break;
            }
        }

        return config;
    }
}

