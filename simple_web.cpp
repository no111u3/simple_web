#include "simple_web.h"
#include "handle_message.h"
#include "pooling.h"
#include "daemon.h"

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

    if (not_daemon) {
        pooling::pool_processor(once, address_config);
    } else {
        auto run = [once, address_config]() { pooling::pool_processor(once, address_config); };
        server::get_daemon(run);
    }
    
    return 0;
}
