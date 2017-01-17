//
// Created by nis on 08.01.17.
//

#ifndef WORK_STEALING_QUEUE_H
#define WORK_STEALING_QUEUE_H

#include "spinlock_mutex.h"
#include <deque>

#include "function_wrapper.h"

namespace util {
    class work_stealing_queue {
    private:
        typedef function_wrapper data_type;
        typedef util::spinlock_mutex mutex_type;
        std::deque<data_type> the_queue;
        mutable mutex_type the_mutex;
    public:
        work_stealing_queue() {}

        work_stealing_queue(const work_stealing_queue &) = delete;

        work_stealing_queue &operator=(const work_stealing_queue &) = delete;

        void push(data_type data) {
            std::lock_guard<mutex_type> lock(the_mutex);
            the_queue.push_front(std::move(data));
        }

        bool empty() const {
            std::lock_guard<mutex_type> lock(the_mutex);
            return the_queue.empty();
        }

        bool try_pop(data_type &res) {
            std::lock_guard<mutex_type> lock(the_mutex);
            if (the_queue.empty())
                return false;

            res = std::move(the_queue.front());
            the_queue.pop_front();
            return true;
        }

        bool try_steal(data_type &res) {
            std::lock_guard<mutex_type> lock(the_mutex);
            if (the_queue.empty()) {
                return false;
            }

            res = std::move(the_queue.back());
            the_queue.pop_back();
            return true;
        }
    };
}
#endif //WORK_STEALING_QUEUE_H
