#include "simple_web.h"
#include "pooling.h"
#include "daemon.h"
#include "config.h"

int main(int argc, char **argv) {
    std::cout << "simple web server" << std::endl;
    sockaddr_in address_config;
    bool not_daemon;
    bool once;

    config::get_config(argc, argv, address_config, not_daemon, once);

    if (not_daemon) {
        pooling::pool_processor(once, address_config);
    } else {
        auto run = [once, address_config]() { pooling::pool_processor(once, address_config); };
        server::get_daemon(run);
    }
    
    return 0;
}
