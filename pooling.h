//
// Created by nis on 23.12.16.
//

#ifndef POOLING_H
#define POOLING_H

#include <sys/socket.h>
#include <fcntl.h>
#include <cerrno>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"

namespace pooling {
    const int polling_size = 16;
    const int run_timeout = -1;

    int pool_processor(conf::Config &config);
}

#endif // POOLING_H
