# High Performance Multithreaded File Transfer Server

A high-performance TCP file transfer system implemented in **C++17** using a **multithreaded thread-pool architecture**. The project demonstrates concurrent client handling, TCP socket programming, synchronization, file I/O, CMake-based builds, automated testing, and performance benchmarking on Linux.

---

## Project Overview

This project implements a TCP-based client-server file transfer system designed to handle multiple clients concurrently.

Instead of creating a new thread for every incoming connection, the server uses a **fixed-size worker thread pool**. Incoming client requests are submitted to a task queue, and available worker threads process the requests.

The project focuses on Linux systems programming concepts commonly used in embedded and performance-sensitive software:

* C++17
* POSIX/Linux networking
* TCP sockets
* Multithreading
* Thread pools
* Mutexes
* Condition variables
* Concurrent task processing
* File I/O
* CMake
* Automated testing
* Performance benchmarking

---

## Key Features

* TCP client-server communication
* Concurrent client handling
* Fixed-size worker thread pool
* Thread-safe task queue
* Condition-variable based worker synchronization
* Large-file transfer support
* File integrity verification using SHA-256
* Configurable number of worker threads
* CMake build system
* Automated ThreadPool unit test
* CTest integration
* Benchmark data collection
* Linux command-line based workflow

---

## System Architecture

```text
                         TCP Client
                             |
                             | TCP Connection
                             v
                    +------------------+
                    |    TCP Server    |
                    +--------+---------+
                             |
                             | Client Request
                             v
                    +------------------+
                    |   Thread Pool    |
                    +--------+---------+
                             |
                +------------+------------+
                |            |            |
                v            v            v
           Worker 1      Worker 2      Worker N
                |            |            |
                +------------+------------+
                             |
                             v
                       File I/O
                             |
                             v
                       TCP Socket
                             |
                             v
                         Client
```

### Request Flow

1. The server creates a TCP listening socket.
2. The server binds the socket to port `8080`.
3. The server starts listening for incoming connections.
4. A client establishes a TCP connection.
5. The server accepts the connection.
6. The client request is submitted as a task to the thread pool.
7. An available worker thread processes the request.
8. The requested file is read and transmitted through the TCP connection.
9. Multiple clients can be processed concurrently by different worker threads.

---

## Thread Pool Design

The server uses a fixed number of worker threads.

```text
                 Task Queue
                     |
        +------------+------------+
        |            |            |
        v            v            v
     Worker 1     Worker 2     Worker 3
        |            |            |
        +------------+------------+
                     |
                 File Transfer
```

The `ThreadPool` contains:

* Worker threads
* A task queue
* A mutex protecting the queue
* A condition variable for worker notification
* A stop flag for controlled shutdown

### Worker synchronization

Workers wait on a condition variable when no tasks are available.

Conceptually:

```cpp
condition.wait(lock, [this] {
    return stop || !tasks.empty();
});
```

This avoids continuously polling the task queue and allows idle worker threads to block until work becomes available.

---

## Technologies Used

| Technology                | Purpose                         |
| ------------------------- | ------------------------------- |
| C++17                     | Core implementation             |
| TCP/IP                    | Network communication           |
| Linux/POSIX APIs          | System-level networking and I/O |
| `std::thread`             | Worker threads                  |
| `std::mutex`              | Thread synchronization          |
| `std::condition_variable` | Worker notification             |
| CMake                     | Build system                    |
| CTest                     | Automated testing               |
| Git/GitHub                | Version control                 |
| Bash                      | Build and test automation       |

---

## Project Structure

```text
High_performance_multithreaded_file_transfer_server/
│
├── docs/
│   └── benchmarks/
│       └── results.csv
│
├── include/
│   ├── TCPClient.h
│   ├── TCPServer.h
│   └── ThreadPool.h
│
├── src/
│   ├── main.cpp
│   ├── server_main.cpp
│   ├── TCPClient.cpp
│   ├── TCPServer.cpp
│   └── ThreadPool.cpp
│
├── tests/
│   └── test_threadpool.cpp
│
├── .gitignore
├── CMakeLists.txt
├── README.md
├── sample.txt
└── phase1.c++.cpp
```

---

## Build Instructions

### Requirements

* Linux environment
* C++17 compatible compiler
* CMake 3.16 or newer

Check the compiler:

```bash
g++ --version
```

Check CMake:

```bash
cmake --version
```

### Build

From the project root:

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

The build generates:

```text
server
client
test
```
