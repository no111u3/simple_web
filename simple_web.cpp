#include "simple_web.h"
#include "pooling.h"
#include "daemon.h"

int main(int argc, char **argv) {
    std::cout << "simple web server" << std::endl;

    auto config = conf::get_config(argc, argv);

    if (config.not_daemon) {
        pooling::pool_processor(config);
    } else {
        auto run = [&config]() { pooling::pool_processor(config); };
        server::get_daemon(run);
    }
    
    return 0;
}
