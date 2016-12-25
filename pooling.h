//
// Created by nis on 23.12.16.
//

#ifndef POOLING_H
#define POOLING_H

#include "config.h"

#include <fcntl.h>
#include <sys/epoll.h>

namespace pooling {
    const int polling_size = 16;
    const int run_timeout = -1;

    inline void set_non_block(int &socket) {
        int flags = fcntl(socket, F_GETFL, 0);
        fcntl(socket, F_SETFL, flags | O_NONBLOCK);
    }

    class Pool final {
    public:
        Pool(conf::Config &config);
        ~Pool();

        int operator() ();
    private:
        conf::Config config_;
        epoll_event ev_;
        int listener_;
        int epooll_fd_;
    };
}

#endif // POOLING_H
