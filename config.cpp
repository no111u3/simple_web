//
// Created by nis on 25.12.16.
//
#include <iostream>
#include <getopt.h>
#include "config.h"

namespace conf {
    Config Config::config;

    void Config::create_config(int argc, char **argv) {

        int res;
        while ((res = getopt(argc, argv, "h:p:d:ns")) != -1) {
            switch (res) {
                case 'h':
                    Config::config.address.sin_addr.s_addr = inet_addr(optarg);
                    break;
                case 'p':
                    Config::config.address.sin_port = htons((unsigned short )std::stoi(optarg));
                    break;
                case 'd':
                    Config::config.directory = optarg;
                    break;
                case 'n':
                    Config::config.not_daemon = true;
                    break;
                case 's':
                    Config::config.once = true;
                    break;
                default:
                    std::cout << "Unknown argument: " << (char)res << std::endl;
                    break;
            }
        }
    }

    Config *Config::get_config() {
        return &Config::config;
    }

    Config::Config() {
        address.sin_family = PF_INET;
        address.sin_port = htons(12345);
        address.sin_addr.s_addr = inet_addr("127.0.0.1");

        not_daemon = false;
        once = false;
    }
}

