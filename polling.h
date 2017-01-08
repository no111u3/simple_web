//
// Created by nis on 23.12.16.
//

#ifndef POOLING_H
#define POOLING_H

#include "config.h"
#include "thread_pool.h"

#include <fcntl.h>
#include <sys/epoll.h>
#include <thread>

namespace polling {
    const int polling_size = 32;
    const int run_timeout = -1;

    inline void set_non_block(int &socket) {
        int flags = fcntl(socket, F_GETFL, 0);
        fcntl(socket, F_SETFL, flags | O_NONBLOCK);
    }

    class Poll final {
    public:
        Poll(conf::Config &config);
        Poll(const Poll &) = delete;
        Poll &operator =(const Poll &) = delete;
        ~Poll();

        int operator() ();
    private:
        void process(const int &descriptor);
        inline void epoll_add(const int &descriptor);
        inline void epoll_del(const int &descriptor);

        conf::Config config_;
        epoll_event ev_;
        int listener_;
        int epoll_fd_;
        util::thread_pool pool;
    };
}

#endif // POOLING_H
