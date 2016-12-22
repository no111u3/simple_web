#include <string>
#include <iostream>
#include <sstream>
#include <list>
#include <fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define EPOLL_SIZE 32
#define BUF_SIZE (4*1024)

#define EPOLL_RUN_TIMEOUT -1
namespace {
    const char *title =
        "<head><title>simple web server</title></head>"
        "<body><h3><center>simple web server</center></h3></body>";
    const char *error404 =
        "<head><title>simple web server</title></head>"
        "<body><h3><center>404 request page not found!</center></h3></body>";
    
    std::string directory = ".";
}
int handle_message(int client) {
    char buf[BUF_SIZE];
    bzero(buf, BUF_SIZE);
    
    int len = recv(client, buf, BUF_SIZE, 0);
    
    if (len != 0) {
        std::stringstream in(buf);
        std::string req_type;
        in >> req_type;
        if (req_type == "GET") {
            std::string req;
            bool success = false;
            int content_len = 0;
            std::string content;
            in >> req;
            // cleaning request
            {
                std::size_t found = req.find("?");
                if (found != std::string::npos) {
                    req = std::string(req.begin(), req.begin()+found);
                }
            }
            if (req == "/") {
                success = true;
                content = title;
                content_len = content.size();
            } else {
                std::ifstream ifs;
                std::stringstream c_stream;
                std::string path = directory + req;
                ifs.open(path.c_str(), std::ifstream::in);
                
                if (ifs.good()) {
                    success = true;
                    char c = ifs.get();
                    while (ifs.good()) {
                        c_stream << c;
                        c = ifs.get();
                    }
                    content = c_stream.str();
                    content_len = content.size();
                } else {
                    content = error404;
                    content_len = content.size();
                }
                
                ifs.close();
            }
            std::stringstream out;
            std::string resp = (success ? "200 Ok" : "404 Not Found");
            out << "HTTP/1.0 " << resp << "\r\n"; 
            out << "Server: simple web\r\n"
                "Content-Type: text/html\r\n";
            out << "Content-Lenght: " << content_len << "\r\n";
            if (content_len) {
                out << "\r\n";
                out << content;
            }
            send(client, out.str().c_str(), out.str().size(), 0);
            std::cout << req_type << ": " << req << " ";
            std::cout << resp << " ";
            std::cout << content_len << std::endl;
        }
    }
    close(client);
    
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

    int listener = socket(PF_INET, SOCK_STREAM, 0);

    int bnd = bind(listener, (struct sockaddr *)&addr, sizeof(addr));

    listen(listener, 1);
    
    int epfd = epoll_create(EPOLL_SIZE);
    
    ev.data.fd = listener;
    
    epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev);
    
    while (true) {
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT);
        
        for (int i = 0; i < epoll_events_count; i++) {
            if (events[i].data.fd == listener) {
                int client = accept(listener, (struct sockaddr *) &their_addr, &socklen);
                
                ev.data.fd = client;
                
                epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev);
            } else {
                int res = handle_message(events[i].data.fd);
            }
        }
    }
    
    close(listener);
    close(epfd);
    
    return 0;
}
