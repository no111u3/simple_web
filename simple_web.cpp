#include <iostream>
#include <list>
#include <fstream>
#include <cstring>
#include <cstdlib>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

#define EPOLL_SIZE 16
#define BUF_SIZE (4*1024)

#define EPOLL_RUN_TIMEOUT -1

#include <thread>

namespace {
    const char title[] =
        "<head><title>simple web server</title></head>"
        "<body><h3><center>simple web server</center></h3></body>";
    const int size_title = sizeof(title) - 1;
    const char error404[] =
        "<head><title>simple web server</title></head>"
        "<body><h3><center>404 request page not found!</center></h3></body>";
    const int size_eror404 = sizeof(error404) - 1;
    
    const char head_ok[] =
        "HTTP/1.0 200 Ok\r\n"
        "Server: simple web\r\n"
        "Content-Type: text/html\r\n"
        "Content-Lenght: ";
    const int size_head_ok = sizeof(head_ok) - 1;
    
    const char head_error404[] = 
        "HTTP/1.0 404 Not Found\r\n"
        "Server: simple web\r\n"
        "Content-Type: text/html\r\n"
        "Content-Lenght: ";
    const int size_head_error404 = sizeof(head_error404) - 1;
        
    std::string directory = ".";
}
int handle_message(int client) {
    char buf[BUF_SIZE] = {0};
    
    int len = recv(client, buf, BUF_SIZE, 0);
    
    if (len != 0) {
        if (buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T') {
            char *first = &buf[4];
            while (*first == ' ') *first++;
            char *second = first;
            while ((*second != '?') && (*second != ' ')) *second++;
            bool transfer_file = false;
            bool success = false;
            int content_len = 0;
            char *memblock = 0;
            if (second - first == 1 && *first == '/') {
                success = true;
                memblock = (char *)title;
                content_len = size_title;
            } else {
                std::string req(first, second);
                std::string path = directory + req;
                
                struct stat statbuf;
                if (stat(path.c_str(), &statbuf) != -1) {
                    std::ifstream ifs;
                    int size = statbuf.st_size;
                    ifs.open(path.c_str(), std::ifstream::in);
                    memblock = new char [size];
                    transfer_file = true;
                    success = true;
                    ifs.read(memblock, size);
                    content_len = size;
                    ifs.close();
                } else {
                    memblock = (char *)error404;
                    content_len = size_eror404;
                }
            }
            if (success) {
                send(client, head_ok, size_head_ok, 0);
            } else {
                send(client, head_error404, size_head_error404, 0);
            }
            char buffer[10+4+1];
            int size = std::sprintf(buffer, "%d\n\n\n\n", content_len);
            send(client, buffer, size, 0);
            if (content_len) {
                send(client, memblock, content_len, 0);
            }
            if (transfer_file)
                delete [] memblock;
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
    bool not_daemon = false;
    
    static struct epoll_event ev, events[EPOLL_SIZE];
    
    ev.events = EPOLLIN;
    int res;
    while ((res = getopt(argc, argv, "h:p:d:n")) != -1) {
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
            case 'n':
                not_daemon = true;
                break;
        }    
    }

    int pid = 0;
    // daemoning
    if (!not_daemon)
        pid = fork();
    
    if (pid == -1)
    {
        std::cout << "Error to start daemon" << std::endl;
    }
    else if (!pid) {
        umask(0);
        setsid();
        chdir("/");
        if (!not_daemon) {
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            
            pid = fork();
        }
        if (!pid) {
            int listener = socket(PF_INET, SOCK_STREAM, 0);
            int flags = fcntl(listener, F_GETFL, 0);
            fcntl(listener, F_SETFL, flags | O_NONBLOCK);

            int yes = 1;
            setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
            
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
    }
    
    return 0;
}
