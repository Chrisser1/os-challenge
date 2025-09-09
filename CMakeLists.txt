cmake_minimum_required(VERSION 3.16)
project(os_challenge C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -g -O2 -march=native)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

include_directories(src)

add_executable(server
        src/main.c
        src/server.c
        src/protocol.c
        src/protocol.h
        src/hashing.c
        src/hashing.h
        src/common/logging.h
        src/thread-pool.c
        src/thread-pool.h
        src/dispatcher_queue.c
        src/dispatcher_queue.h
        src/cache.c
        src/cache.h)

target_include_directories(server PRIVATE
    src
)

target_link_libraries(server PRIVATE
        OpenSSL::Crypto
        Threads::Threads
)

# Place the final "server" executable in the project's root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})