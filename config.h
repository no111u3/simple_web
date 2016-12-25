//
// Created by nis on 25.12.16.
//

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

namespace conf {
    struct Config {
        sockaddr_in address;
        bool not_daemon;
        bool once;

        std::string directory;
    };
    Config get_config(int argc, char **argv);
}
#endif //PROJECT_CONFIG_H
