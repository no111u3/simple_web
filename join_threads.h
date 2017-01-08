//
// Created by nis on 08.01.17.
//

#ifndef JOIN_THREADS_H
#define JOIN_THREADS_H

#include <thread>
#include <vector>

namespace util {

    class join_threads {
        std::vector<std::thread> &threads;
    public:
        explicit join_threads(std::vector<std::thread> &threads_) :
                threads(threads_) {}

        ~join_threads() {
            for (unsigned long i = 0; i < threads.size(); ++i) {
                if (threads[i].joinable())
                    threads[i].join();
            }
        }
    };
}

#endif //JOIN_THREADS_H
