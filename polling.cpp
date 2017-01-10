//
// Created by nis on 23.12.16.
//

#include "polling.h"
#include "handle_message.h"

#include <iostream>

#include <unistd.h>

#include "core.h"

namespace polling {
    Poll::Poll() {
        ev_.events = EPOLLIN;

        listener_ = socket(PF_INET, SOCK_STREAM, 0);
        set_non_block(listener_);

        int yes = 1;
        setsockopt(listener_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(int));

        bind(listener_, (sockaddr *)&(conf::Config::get_config()->address),
             sizeof(conf::Config::get_config()->address));

        listen(listener_, ECONNREFUSED);

        epoll_fd_ = epoll_create(polling_size);

        ev_.data.fd = listener_;

        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listener_, &ev_);
    }

    Poll::~Poll() {
        close(listener_);
        close(epoll_fd_);
    }

    int Poll::operator()() {
        epoll_event events[polling_size];
        bool &once = conf::Config::get_config()->once;

        int events_count = epoll_wait(epoll_fd_, events, polling_size, run_timeout);

        if (events_count == -1) {
            std::cout << "Epool working error" << std::endl;
            once = false;
        }
        for (int i = 0; i < events_count; i++) {
            int descriptor = events[i].data.fd;
            if (descriptor == listener_) {
                socklen_t socklen;
                socklen = sizeof(sockaddr_in);
                sockaddr_in their_addr;
                int client = accept(listener_, (sockaddr *) &their_addr, &socklen);
                set_non_block(client);

                ev_.data.fd = client;

                epoll_add(client);
            } else {
                server::Core::get_pool()->submit([descriptor] {
                    http::handle_message(std::move(descriptor));
                });
                epoll_del(descriptor);
            }
        }

        return 0;
    }

    void Poll::epoll_add(const int &descriptor) {
        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, descriptor, &ev_);
    }

    void Poll::epoll_del(const int &descriptor) {
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, descriptor, nullptr);
    }
}
