//
// Created by nis on 23.12.16.
//

#include "pooling.h"
#include "handle_message.h"

#include <thread>
#include <iostream>

#include <unistd.h>

namespace pooling {
    Pool::Pool(conf::Config &config) : config_(config) {
        ev_.events = EPOLLIN;

        listener_ = socket(PF_INET, SOCK_STREAM, 0);
        set_non_block(listener_);

        int yes = 1;
        setsockopt(listener_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        bind(listener_, (sockaddr *)&(config_.address), sizeof(config_.address));

        listen(listener_, ECONNREFUSED);

        epooll_fd_ = epoll_create(polling_size);

        ev_.data.fd = listener_;

        epoll_ctl(epooll_fd_, EPOLL_CTL_ADD, listener_, &ev_);
    }

    Pool::~Pool() {
        close(listener_);
        close(epooll_fd_);
    }

    int Pool::operator()() {
        epoll_event events[polling_size];

        while (!config_.once) {
            int events_count = epoll_wait(epooll_fd_, events, polling_size, run_timeout);

            if (events_count == -1) {
                std::cout << "Epool working error" << std::endl;
                config_.once = false;
            }

            for (int i = 0; i < events_count; i++) {
                if (events[i].data.fd == listener_) {
                    socklen_t socklen;
                    socklen = sizeof(sockaddr_in);
                    sockaddr_in their_addr;
                    int client = accept(listener_, (sockaddr *) &their_addr, &socklen);
                    set_non_block(client);

                    ev_.data.fd = client;

                    epoll_ctl(epooll_fd_, EPOLL_CTL_ADD, client, &ev_);
                } else {
                    epoll_ctl(epooll_fd_, EPOLL_CTL_DEL, events[i].data.fd, &ev_);
                    int client = events[i].data.fd;
                    std::thread t{http::handle_message, client, config_};
                    t.detach();
                }
            }
        }

        return 0;
    }
}
