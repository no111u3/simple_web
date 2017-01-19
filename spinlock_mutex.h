//
// Created by nis on 17.01.17.
//

#ifndef SPINLOCK_MUTEX_H
#define SPINLOCK_MUTEX_H

#include <atomic>

namespace util {

    class spinlock_mutex {
        std::atomic_flag flag;
    public:
        spinlock_mutex() :
                flag(ATOMIC_FLAG_INIT) {}

        void lock() {
            while (flag.test_and_set(std::memory_order_acquire));
        }

        bool try_lock() {
            return !flag.test_and_set(std::memory_order_acquire);
        }

        void unlock() {
            flag.clear(std::memory_order_release);
        }
    };
}

#endif //SPINLOCK_MUTEX_H
