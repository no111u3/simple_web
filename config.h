//
// Created by nis on 25.12.16.
//

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include <netinet/in.h>
#include <arpa/inet.h>

namespace config {
    void get_config(int argc, char **argv, sockaddr_in &address_config, bool &not_daemon, bool &once);
}
#endif //PROJECT_CONFIG_H
