//
// Created by nis on 03.01.17.
//

#ifndef THREADSAFE_QUEUE_H
#define THREADSAFE_QUEUE_H

#include <memory>

#include <queue>
#include "spinlock_mutex.h"

namespace util {
    template <typename T>
    class thread_safe_queue {
    private:
        struct node {
            std::shared_ptr<T> data;
            std::unique_ptr<node> next;
        };
        typedef util::spinlock_mutex mutex_type;
        mutable mutex_type head_mutex;
        std::unique_ptr<node> head;
        mutable mutex_type tail_mutex;
        node *tail;

        node * get_tail() {
            std::lock_guard<mutex_type> tail_lock(tail_mutex);
            return tail;
        }

        inline std::unique_ptr<node> pop_head() {
            std::unique_ptr<node> old_head = std::move(head);
            head = std::move(old_head->next);
            return std::move(old_head);
        }

        std::unique_ptr<node> try_pop_head(T &value) {
            std::lock_guard<mutex_type> head_lock(head_mutex);
            if (head.get() == get_tail()) {
                return std::unique_ptr<node>();
            }
            value = std::move(*head->data);
            return std::move(pop_head());
        }

    public:
        thread_safe_queue() : head(new node), tail(head.get()) {}
        thread_safe_queue(const thread_safe_queue &other) = delete;
        thread_safe_queue & operator = (const thread_safe_queue &) = delete;

        bool try_pop(T &value) {
            std::unique_ptr<node> old_head = try_pop_head(value);
            return old_head != nullptr;
        }

        void push(T new_value) {
            std::shared_ptr<T> new_data(
                std::make_shared<T>(std::move(new_value)));
            std::unique_ptr<node> p(new node);
            {
                node *const new_tail = p.get();
                std::lock_guard<mutex_type> tail_lock(tail_mutex);
                tail->data = new_data;
                tail->next = std::move(p);
                tail = new_tail;
            }
        }

        void empty() {
            std::lock_guard<mutex_type> head_lock(head_mutex);
            return (head.get() == get_tail());
        }
    };
}

#endif //PROJECT_THREADSAFE_QUEUE_H
