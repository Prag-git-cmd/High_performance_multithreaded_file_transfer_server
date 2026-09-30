# High Performance Multithreaded File Transfer Server

A Linux-based TCP file transfer server implemented in **C++17** using a fixed-size **thread pool** for concurrent client handling.

The project demonstrates Linux systems programming, TCP socket programming, multithreading, synchronization, file I/O, CMake, automated testing, benchmarking, and zero-copy file transfer using Linux `sendfile()`.

---

## Project Overview

The server accepts multiple TCP client connections and processes file-transfer requests using a fixed-size worker thread pool.

Instead of creating a new thread for every client, incoming connections are submitted to a task queue and processed by available worker threads.

### Core concepts demonstrated

- C++17
- Linux/POSIX APIs
- TCP sockets
- Multithreading
- Thread pools
- Mutexes
- Condition variables
- Producer-consumer synchronization
- File I/O
- Zero-copy `sendfile()`
- CMake
- CTest
- Performance benchmarking
- SHA-256 file integrity verification

---

## Key Features

- TCP client-server file transfer
- Concurrent client handling
- Configurable worker-thread count
- Thread-safe task queue
- Condition-variable based worker synchronization
- Large-file transfer support
- Buffered file transfer
- Linux `sendfile()` zero-copy transfer
- Runtime transfer-mode selection
- SHA-256 integrity verification
- Automated ThreadPool test
- CTest integration
- Benchmark data stored in CSV format

---

## System Architecture

```text
                         TCP Client
                              |
                              | TCP Connection
                              v
                    +-------------------+
                    |    TCP Server     |
                    +---------+---------+
                              |
                              | Accept Client
                              v
                    +-------------------+
                    |    Thread Pool    |
                    +---------+---------+
                              |
                +-------------+-------------+
                |             |             |
                v             v             v
             Worker 1      Worker 2      Worker N
                |             |             |
                +-------------+-------------+
                              |
                              v
                       File Transfer
                              |
                 +------------+------------+
                 |                         |
                 v                         v
          Buffered I/O              sendfile()
                                      zero-copy
                 |                         |
                 +------------+------------+
                              |
                              v
                         TCP Socket
                              |
                              v
                           Client
