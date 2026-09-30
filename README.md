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
test_threadpool
```

---

## Running the Server

From the `build` directory:

```bash
./server
```

The default configuration starts the server with 3 worker threads.

You can specify the number of worker threads:

```bash
./server 4
```

For example:

```text
Starting server with 4 worker threads
Socket created successfully
Socket bound to port 8080
Server is listening...
```

---

## Running the Client

The client supports file download operations.

Example:

```bash
./client download large_file.bin downloaded.bin
```

A successful transfer reports information such as:

```text
Connected to server
Downloaded 104857600 bytes
Transfer time: 0.651 seconds
Throughput: 153.61 MB/s
```

The exact throughput depends on the execution environment, network stack, system load, and concurrent clients.

---

## Generating a Large Test File

A 100 MB test file can be generated using:

```bash
dd if=/dev/zero of=large_file.bin bs=1M count=100
```

The generated file is intentionally excluded from Git using `.gitignore`.

---

## File Integrity Verification

Performance alone is not sufficient for a file-transfer system. The transferred file must also match the original.

### Compare files

```bash
cmp large_file.bin downloaded.bin
```

No output from `cmp` indicates that the files are identical.

### SHA-256 verification

Calculate the checksum of the source:

```bash
sha256sum large_file.bin
```

Calculate the checksum of the downloaded file:

```bash
sha256sum downloaded.bin
```

The two SHA-256 values should match.

Example:

```text
20492a4d0d84f8beb1767f6616229f85d44c2827b64bdbfb260ee12fa1109e0e  large_file.bin

20492a4d0d84f8beb1767f6616229f85d44c2827b64bdbfb260ee12fa1109e0e  downloaded.bin
```

This verifies that the transferred data is identical to the original file.

---

## Testing

The project includes a ThreadPool unit test.

Run the test executable directly:

```bash
./build/test_threadpool
```

Expected result:

```text
ThreadPool test PASSED
Completed tasks: 100/100
```

The project also integrates the test with CTest.

From the build directory:

```bash
ctest --output-on-failure
```

Expected result:

```text
1/1 Test #1: ThreadPoolTest ............ Passed

100% tests passed, 0 tests failed
```

The test validates that 100 submitted tasks are successfully executed by the thread pool.

---

## Performance Benchmark

The server was tested with multiple concurrent clients transferring a 100 MB file per client.

Measured benchmark results:

| Clients | Total Data | Total Time | Aggregate Throughput |
| ------: | ---------: | ---------: | -------------------: |
|       1 |     100 MB |     0.76 s |          131.58 MB/s |
|       3 |     300 MB |     1.98 s |          151.52 MB/s |
|       5 |     500 MB |     4.12 s |          121.36 MB/s |

Benchmark data is stored in:

```text
docs/benchmarks/results.csv
```

### Benchmark interpretation

The results demonstrate that the server can process multiple client transfers concurrently.

Throughput does not necessarily increase linearly with the number of clients because overall performance depends on factors such as:

* CPU scheduling
* Network bandwidth
* TCP stack behavior
* File-system performance
* Disk/cache behavior
* Number of worker threads
* System resource contention
* Execution environment

Therefore, the benchmark values represent measurements from the test environment rather than guaranteed maximum performance.

---

## Concurrency Model

The server uses a fixed worker-thread model rather than creating an unlimited number of threads.

```text
Incoming Connections
        |
        v
+-------------------+
|   Task Queue      |
+---------+---------+
          |
          v
+-------------------------+
|     Worker Threads      |
|                         |
| T1   T2   T3   ...  TN |
+-------------------------+
          |
          v
     File Transfer
```

### Why a thread pool?

Creating a new thread for every request can introduce additional overhead when the number of connections increases.

A thread pool provides:

* Controlled thread count
* Thread reuse
* Reduced thread creation overhead
* Centralized task scheduling
* Better control over system resources

---

## Synchronization

The task queue is shared between the producer and worker threads.

Synchronization is required to prevent race conditions.

The implementation uses:

### Mutex

Protects access to the shared task queue.

```cpp
std::mutex queueMutex;
```

### Condition Variable

Allows worker threads to sleep when no work is available and wake when a new task arrives.

```cpp
std::condition_variable condition;
```

### Stop Flag

The pool maintains a shutdown state:

```cpp
bool stop;
```

This allows worker threads to terminate cleanly when the `ThreadPool` object is destroyed.

---

## Design Considerations

### Fixed worker count

The number of workers can be configured when starting the server:

```bash
./server 4
```

This allows experimentation with different concurrency levels.

### Thread-safe task queue

Client requests are converted into tasks and inserted into the shared queue under mutex protection.

### Blocking synchronization

Workers wait on a condition variable instead of continuously checking whether work exists.

This avoids unnecessary CPU consumption while the server is idle.

### Graceful shutdown

The ThreadPool destructor signals workers to stop and allows the worker threads to terminate cleanly.

---

## Error Handling

The server performs validation during startup and socket initialization, including:

* Socket creation
* Socket binding
* Listening
* Client connection handling
* Worker thread initialization

The server reports failures through standard error output and terminates when startup cannot be completed successfully.

---

## Example Workflow

### Terminal 1 — Start server

```bash
./build/server 4
```

### Terminal 2 — Transfer file

```bash
./build/client download large_file.bin downloaded.bin
```

### Terminal 3 — Verify integrity

```bash
cmp large_file.bin downloaded.bin
```

Then:

```bash
sha256sum large_file.bin downloaded.bin
```

### Run automated tests

```bash
cd build
ctest --output-on-failure
```

---

## What This Project Demonstrates

This project provides practical experience with:

* C++17 programming
* Linux systems programming
* TCP socket programming
* Multithreaded application design
* Thread pools
* Producer-consumer synchronization
* Mutexes
* Condition variables
* Concurrent task execution
* File I/O
* Network data transfer
* Performance measurement
* CMake
* CTest
* Git/GitHub
* Linux command-line development

These concepts are directly relevant to systems software and Embedded Linux development.

---

## Future Improvements

Potential extensions include:

* `sendfile()` based zero-copy file transfer
* Configurable socket buffer sizes
* Transfer rate limiting
* Connection timeout handling
* Improved request/response protocol
* Graceful server shutdown through signals
* Connection pooling
* Per-client performance metrics
* Structured logging
* Error recovery and retry mechanisms
* Benchmark automation
* CPU and memory profiling
* Linux `epoll` based event-driven networking
* TLS-secured file transfer

---

## Author

**Pragyanshree Sahoo**

Embedded Linux / C++ Software Engineering

---

## License

This project is intended as a systems-programming and learning project for demonstrating Linux, C++, networking, multithreading, and performance-engineering concepts.
