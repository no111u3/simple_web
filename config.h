//
// Created by nis on 25.12.16.
//

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

namespace conf {
    struct Config final {
        sockaddr_in address;
        bool not_daemon;
        bool once;

        std::string directory;

        Config(const Config &) = delete;
        Config &operator = (const Config &) = delete;
        static void create_config(int argc, char **argv);

        static Config * get_config();
    private:
        Config();

        static Config config;
    };
}
#endif //PROJECT_CONFIG_H
