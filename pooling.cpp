//
// Created by nis on 23.12.16.
//

#include "pooling.h"
#include "handle_message.h"

#include <thread>
#include <iostream>

namespace pooling {

    int pool_processor(conf::Config &config) {
        socklen_t socklen;
        socklen = sizeof(sockaddr_in);
        sockaddr_in their_addr;
        epoll_event ev, events[polling_size];

        ev.events = EPOLLIN;

        int listener = socket(PF_INET, SOCK_STREAM, 0);
        {
            int flags = fcntl(listener, F_GETFL, 0);
            fcntl(listener, F_SETFL, flags | O_NONBLOCK);
        }
        int yes = 1;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        bind(listener, (sockaddr *)&(config.address), sizeof(config.address));

        listen(listener, ECONNREFUSED);

        int epooll_fd = epoll_create(polling_size);

        ev.data.fd = listener;

        epoll_ctl(epooll_fd, EPOLL_CTL_ADD, listener, &ev);

        while (!config.once) {
            int events_count = epoll_wait(epooll_fd, events, polling_size, run_timeout);

            if (events_count == -1) {
                std::cout << "Epool working error" << std::endl;
                config.once = false;
            }

            for (int i = 0; i < events_count; i++) {
                if (events[i].data.fd == listener) {
                    int client = accept(listener, (sockaddr *) &their_addr, &socklen);
                    {
                        int flags = fcntl(client, F_GETFL, 0);
                        fcntl(client, F_SETFL, flags | O_NONBLOCK);
                    }
                    ev.data.fd = client;

                    epoll_ctl(epooll_fd, EPOLL_CTL_ADD, client, &ev);
                } else {
                    epoll_ctl(epooll_fd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                    int client = events[i].data.fd;
                    std::thread t([client, &config](){
                        http::handle_message(client, config);
                    });
                    t.detach();
                }
            }
        }
        close(listener);
        close(epooll_fd);
        return 0;
    }
}
