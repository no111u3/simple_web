//
// Created by nis on 10.01.17.
//

#include "core.h"

namespace server {
    Core Core::core;

    void Core::operator()() {
        pool.reset(new util::thread_pool);
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    Core *Core::get_core() {
        return &Core::core;
    }

    std::shared_ptr<util::thread_pool> Core::get_pool() {
        return Core::core.pool;
    }
}
