#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <list>

#define EPOLL_SIZE 4
#define MSG_SIZE 255

#define EPOLL_RUN_TIMEOUT -1

std::list<int> clients;

int handle_message(int client) {
    char buf[MSG_SIZE];
    bzero(buf, MSG_SIZE);
    
    int len = recv(client, buf, MSG_SIZE, 0);
    
    if (len == 0) {
        close(client);
        clients.remove(client);
    } else {
        send(client, buf, MSG_SIZE, 0);
    }
    
    return len;
}

int main() {
    std::cout << "simple web server" << std::endl;
    struct sockaddr_in addr, their_addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t socklen;
    socklen = sizeof(struct sockaddr_in);
    
    static struct epoll_event ev, events[EPOLL_SIZE];
    
    ev.events = EPOLLIN | EPOLLET;
    
    char message[MSG_SIZE + 1];
    
    int epfd;
    
    int listener = socket(PF_INET, SOCK_STREAM, 0);

    int bnd = bind(listener, (struct sockaddr *)&addr, sizeof(addr));

    listen(listener, 1);
    
    epfd = epoll_create(EPOLL_SIZE);
    
    ev.data.fd = listener;
    
    epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev);
    
    while (true) {
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT);
        
        for (int i = 0; i < epoll_events_count; i++) {
            if (events[i].data.fd == listener) {
                int client = accept(listener, (struct sockaddr *) &their_addr, &socklen);
                
                ev.data.fd = client;
                
                epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev);
                clients.push_back(client);
            } else {
                int res = handle_message(events[i].data.fd);
            }
        }
    }
    
    close(listener);
    close(epfd);
    
    return 0;
}
