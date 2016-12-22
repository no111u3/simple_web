#include <string>
#include <iostream>
#include <sstream>
#include <list>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define EPOLL_SIZE 4
#define BUF_SIZE (4*1024)

#define EPOLL_RUN_TIMEOUT -1

#include <thread>

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
    char buf[BUF_SIZE] = {0};
    
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
            char *memblock = 0;
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
                std::string path = directory + req;
                
                struct stat statbuf;
                if (stat(path.c_str(), &statbuf) != -1) {
                    int size = statbuf.st_size;
                    ifs.open(path.c_str(), std::ifstream::in);
                    memblock = new char [size];
                    ifs.read(memblock, size);
                    success = true;
                    content_len = size;
                    delete [] memblock;
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
            }
            send(client, out.str().c_str(), out.str().size(), 0);
            if (content_len) {
                if (memblock) {
                    send(client, memblock, content_len, 0);
                } else {
                    send(client, content.c_str(), content_len, 0);
                }
            }
            /*std::cout << req_type << ": " << req << " ";
            std::cout << resp << " ";
            std::cout << content_len << std::endl;*/
        }
    }
    close(client);
    
    return len;
}

int main(int argc, char **argv) {
    std::cout << "simple web server" << std::endl;
    struct sockaddr_in addr, their_addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t socklen;
    socklen = sizeof(struct sockaddr_in);
    
    static struct epoll_event ev, events[EPOLL_SIZE];
    
    ev.events = EPOLLIN;
    int res;
    while ((res = getopt(argc, argv, "h:p:d:")) != -1) {
        switch (res) {
            case 'h':
                addr.sin_addr.s_addr = inet_addr(optarg);
                break;
            case 'p':
                addr.sin_port = htons(std::stoi(optarg));
                break;
            case 'd':
                directory = optarg;
                break;
        }    
    }

    int pid = 0;
    // daemoning
    pid = fork();
    
    if (pid == -1)
    {
        std::cout << "Error to start daemon" << std::endl;
    }
    else if (!pid) {
        umask(0);
        setsid();
        chdir("/");
        
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        
        int listener = socket(PF_INET, SOCK_STREAM, 0);
        int flags = fcntl(listener, F_GETFL, 0);
        fcntl(listener, F_SETFL, flags | O_NONBLOCK);

        int bnd = bind(listener, (struct sockaddr *)&addr, sizeof(addr));

        listen(listener, ECONNREFUSED);
        
        int epfd = epoll_create(EPOLL_SIZE);
        
        ev.data.fd = listener;
        
        epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev);
        
        while (true) {
            int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT);
            
            for (int i = 0; i < epoll_events_count; i++) {
                if (events[i].data.fd == listener) {
                    int client = accept(listener, (struct sockaddr *) &their_addr, &socklen);
                    
                    int flags = fcntl(client, F_GETFL, 0);
                    fcntl(client, F_SETFL, flags | O_NONBLOCK);
                    
                    ev.data.fd = client;
                    
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev);
                } else {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                    int client = events[i].data.fd;
                    std::thread t([client](){
                        int res = handle_message(client);
                    });
                    t.detach();
                }
            }
        }
        close(listener);
        close(epfd);
    }
    
    return 0;
}
