#include "simple_web.h"
#include "core.h"
#include "daemon.h"

int main(int argc, char **argv) {
    std::cout << "simple web server" << std::endl;

    conf::Config::create_config(argc, argv);



    if (conf::Config::get_config()->not_daemon) {
        server::Core *core = server::Core::get_core();
        (*core)();
    } else {
        server::get_daemon<server::Core>();
    }
    
    return 0;
}
