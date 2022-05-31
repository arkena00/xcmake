# project
cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0091 NEW)
project(xcmake LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)

# target
add_executable(xcmake
        source/main.cpp
        source/xc/xcmake.cpp
        )

set(REPROC++ ON)
add_subdirectory(third_party/reproc)

target_include_directories(xcmake PRIVATE
        include
        third_party/reproc/reproc++/include)

target_link_libraries(xcmake PRIVATE reproc++)