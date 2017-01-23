cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_FLAGS "-Wall -std=c++11 -pthread")

set (CMAKE_BUILD_TYPE Debug)

set (sourses
        simple_web.cpp
        handle_message.cpp
        polling.cpp
        config.cpp
        thread_pool.cpp
        core.cpp uint_to_str.cpp)

add_executable(simple_web ${sourses})
