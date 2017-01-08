#include "simple_web.h"
#include "polling.h"
#include "daemon.h"

int main(int argc, char **argv) {
    std::cout << "simple web server" << std::endl;

    auto config = conf::get_config(argc, argv);

    if (config.not_daemon) {
        polling::Poll poll(config);
        poll();
    } else {
        server::get_daemon<polling::Poll>(config);
    }
    
    return 0;
}
