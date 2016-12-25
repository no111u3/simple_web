//
// Created by nis on 25.12.16.
//
#include <iostream>
#include <getopt.h>
#include "config.h"

#include "handle_message.h"

namespace config {

    void get_config(int argc, char **argv, sockaddr_in &address_config, bool &not_daemon, bool &once) {
        address_config.sin_family = PF_INET;
        address_config.sin_port = htons(12345);
        address_config.sin_addr.s_addr = inet_addr("127.0.0.1");

        not_daemon = false;
        once = false;

        int res;
        while ((res = getopt(argc, argv, "h:p:d:ns")) != -1) {
            switch (res) {
                case 'h':
                    address_config.sin_addr.s_addr = inet_addr(optarg);
                    break;
                case 'p':
                    address_config.sin_port = htons((unsigned short )std::stoi(optarg));
                    break;
                case 'd':
                    http::directory = optarg;
                    break;
                case 'n':
                    not_daemon = true;
                    break;
                case 's':
                    once = true;
                    break;
                default:
                    std::cout << "Unknown argument: " << (char)res << std::endl;
                    break;
            }
        }
    }
}

