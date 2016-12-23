#include "simple_web.h"
#include "handle_message.h"
#include "pooling.h"

int main(int argc, char **argv) {
    std::cout << "simple web server" << std::endl;
    sockaddr_in address_config;
    address_config.sin_family = PF_INET;
    address_config.sin_port = htons(12345);
    address_config.sin_addr.s_addr = inet_addr("127.0.0.1");
    bool not_daemon = false;
    bool once = false;

    int res;
    while ((res = getopt(argc, argv, "h:p:d:ns")) != -1) {
        switch (res) {
            case 'h':
                address_config.sin_addr.s_addr = inet_addr(optarg);
                break;
            case 'p':
                address_config.sin_port = htons((unsigned short )std::stoi(optarg));
                break;
            case 'd':
                http::directory = optarg;
                break;
            case 'n':
                not_daemon = true;
                break;
            case 's':
                once = true;
                break;
            default:
                std::cout << "Unknown argument: " << (char)res << std::endl;
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
            pooling::pool_processor(once, address_config);
        }
    }
    
    return 0;
}
