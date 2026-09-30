cmake_minimum_required(VERSION 3.16)

project(HighPerformanceMultithreadedFileTransferServer
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Header files
include_directories(include)

# Server
add_executable(server
    src/server_main.cpp
    src/TCPServer.cpp
    src/ThreadPool.cpp
)

# Client
add_executable(client
    src/main.cpp
    src/TCPClient.cpp
)

# ThreadPool unit test
enable_testing()

add_executable(test_threadpool
    tests/test_threadpool.cpp
    src/ThreadPool.cpp
)

add_test(
    NAME ThreadPoolTest
    COMMAND test_threadpool
)
