cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_FLAGS "-std=c++11 -pthread")

set (sourses
        simple_web.cpp
        handle_message.cpp
        pooling.cpp
        config.cpp
        )

add_executable(simple_web ${sourses})
