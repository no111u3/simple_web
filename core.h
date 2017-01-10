//
// Created by nis on 10.01.17.
//

#ifndef CORE_H
#define CORE_H

#include "thread_pool.h"
#include "polling.h"

namespace server {

    class Core {
    private:
        Core() {}

        static Core core;
        std::shared_ptr<util::thread_pool> pool;

    public:
        Core(const Core &) = delete;
        Core &operator =(const Core &) = delete;
        static Core *get_core();
        static std::shared_ptr<util::thread_pool> get_pool();
        ~Core() {}

        void operator ()();
    };

}
#endif //PROJECT_CORE_H
