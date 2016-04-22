cmake_minimum_required(VERSION 3.2)
project(libcoronio)

find_package(Threads)

set(CMAKE_C_FLAGS -std=gnu99)
include_directories(include)

add_subdirectory(lib)

link_libraries(coronio)

add_executable(run_server server.c)

add_executable(run_client csapp.c client.c)

target_link_libraries(run_server ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(run_client ${CMAKE_THREAD_LIBS_INIT})
