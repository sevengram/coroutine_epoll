cmake_minimum_required(VERSION 3.2)
project(libcoronio)

find_package(Threads)

set(CMAKE_C_FLAGS -std=gnu99)
include_directories(include)

add_subdirectory(lib)

link_libraries(coronio)

add_executable(run_co_server test_co_server.c)

add_executable(run_pt_server csapp.c test_pt_server.c)

add_executable(run_client csapp.c client.c)

add_executable(sem_test sem_test.c)

add_executable(mutex_test mutex_test.c)

add_executable(cond_test cond_test.c)

target_link_libraries(run_pt_server ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(run_co_server ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(run_client ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(sem_test ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(mutex_test ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(cond_test ${CMAKE_THREAD_LIBS_INIT})
