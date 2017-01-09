#include "simple_web.h"
#include "polling.h"
#include "daemon.h"

int main(int argc, char **argv) {
    std::cout << "simple web server" << std::endl;

    conf::Config::create_config(argc, argv);

    if (conf::Config::get_config()->not_daemon) {
        polling::Poll poll;
        poll();
    } else {
        server::get_daemon<polling::Poll>();
    }
    
    return 0;
}
