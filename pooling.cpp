//
// Created by nis on 23.12.16.
//

#include "pooling.h"
#include "handle_message.h"

#include <iostream>

#include <unistd.h>

namespace polling {
    Poll::Poll(conf::Config &config) : config_(config) {
        ev_.events = EPOLLIN;

        listener_ = socket(PF_INET, SOCK_STREAM, 0);
        set_non_block(listener_);

        int yes = 1;
        setsockopt(listener_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        bind(listener_, (sockaddr *)&(config_.address), sizeof(config_.address));

        listen(listener_, ECONNREFUSED);

        epoll_fd_ = epoll_create(polling_size);

        ev_.data.fd = listener_;

        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listener_, &ev_);

        for (int i = 0; i != polling_size / 4; i++)
            threads.push_back(std::thread(&Poll::process, this));
    }

    Poll::~Poll() {
        std::cout << "WAT" << std::endl;
        close(listener_);
        close(epoll_fd_);
        for (int i = 0; i != polling_size / 4; i++) {
            threads[i].join();
        }
    }

    int Poll::operator()() {
        epoll_event events[polling_size];

        while (!config_.once) {
            int events_count = epoll_wait(epoll_fd_, events, polling_size, run_timeout);

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

                    epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client, &ev_);
                } else {
                    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, events[i].data.fd, &ev_);
                    queue_.push(events[i].data.fd);
                }
            }
        }

        return 0;
    }

    void Poll::process() {
        while (!config_.once) {
            int descriptor;
            queue_.wait_and_pop(descriptor);
            http::handle_message(descriptor, config_);
        }
    }
}
