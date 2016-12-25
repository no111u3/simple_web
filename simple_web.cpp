#include "simple_web.h"
#include "pooling.h"
#include "daemon.h"

int main(int argc, char **argv) {
    std::cout << "simple web server" << std::endl;

    auto config = conf::get_config(argc, argv);

    pooling::Pool pool(config);

    if (config.not_daemon) {
        pool();
    } else {
        server::get_daemon(pool);
    }
    
    return 0;
}
