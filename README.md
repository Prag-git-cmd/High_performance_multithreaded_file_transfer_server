# High Performance Multithreaded File Transfer Server

A Linux-based TCP file transfer server implemented in **C++17** using a fixed-size **thread pool** for concurrent client handling.

The project demonstrates Linux systems programming, TCP socket programming, multithreading, synchronization, file I/O, CMake, automated testing, performance benchmarking, and Linux `sendfile()`-based file transfer.

---

## Project Overview

The server accepts multiple TCP client connections and processes file-transfer requests using a fixed-size worker thread pool.

Instead of creating a new thread for every client, incoming client connections are submitted as tasks to a thread-safe queue. Worker threads retrieve and process these tasks concurrently.

The project focuses on understanding how a multithreaded Linux server can handle multiple file-transfer clients while maintaining controlled resource usage.

### Core Concepts

* C++17
* Linux/POSIX APIs
* TCP socket programming
* Multithreading
* Thread pools
* Mutexes
* Condition variables
* Producer-consumer synchronization
* File I/O
* Buffered file transfer
* Linux `sendfile()`
* Zero-copy file transfer
* CMake
* CTest
* Performance benchmarking
* SHA-256 file integrity verification

---

## Key Features

* TCP client-server file transfer
* Concurrent client handling
* Configurable worker-thread count
* Fixed-size thread pool
* Thread-safe task queue
* Condition-variable based worker synchronization
* Large-file transfer support
* Buffered file transfer
* Linux `sendfile()` file transfer
* Runtime transfer-mode selection
* Automated ThreadPool testing
* CTest integration
* Benchmark results stored in CSV format
* File integrity verification using SHA-256

---

## System Architecture

```text
                         TCP Client
                              |
                              | TCP Connection
                              v
                    +-------------------+
                    |    TCP Server     |
                    |    accept() loop  |
                    +---------+---------+
                              |
                              | Submit client task
                              v
                    +-------------------+
                    |    Thread Pool    |
                    |    Task Queue     |
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
                                      Linux
                                    zero-copy
                 |                         |
                 +------------+------------+
                              |
                              v
                         TCP Socket
                              |
                              v
                           Client
```

### Request Processing Flow

1. The server creates a TCP listening socket.
2. The server waits for incoming client connections using `accept()`.
3. A client connection is submitted to the thread pool.
4. An available worker thread retrieves the task from the queue.
5. The worker processes the client's file-transfer request.
6. File data is transferred using either buffered I/O or Linux `sendfile()`.
7. The client receives the file.
8. The connection is completed and the worker becomes available for another task.

---

## Thread Pool Design

The server uses a **fixed-size thread pool** rather than creating a new thread for every incoming connection.

The thread pool consists of:

* A fixed number of worker threads
* A shared task queue
* A mutex protecting the queue
* A condition variable used to notify waiting workers

### Worker lifecycle

```text
Worker Thread
     |
     v
Wait for task
     |
     v
Condition variable
     |
     v
Task available
     |
     v
Remove task from queue
     |
     v
Process client
     |
     v
Return to waiting state
```

This approach avoids repeatedly creating and destroying threads for each client and provides controlled concurrency.

---

## File Transfer Modes

The server supports two file-transfer approaches.

### 1. Buffered File Transfer

The traditional approach reads file data into an application buffer and then sends the data through the TCP socket.

```text
Disk
  |
  v
read()
  |
  v
User-space buffer
  |
  v
send()
  |
  v
TCP Socket
```

This approach provides a straightforward implementation and makes the data flow explicit.

### 2. Linux `sendfile()`

The project also supports Linux `sendfile()` for file transmission.

```text
Disk
  |
  v
sendfile()
  |
  v
TCP Socket
  |
  v
Client
```

`sendfile()` allows the kernel to transfer file data directly between the file descriptor and socket, reducing unnecessary application-level data copying.

This is useful for understanding **zero-copy-oriented I/O and Linux performance optimization**.

---

## Project Structure

```text
High_performance_multithreaded_file_transfer_server/
│
├── CMakeLists.txt
├── README.md
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
│   ├── TCPClient.cpp
│   ├── TCPServer.cpp
│   ├── ThreadPool.cpp
│   ├── main.cpp
│   └── server_main.cpp
│
└── tests/
    └── test_threadpool.cpp
```

### Main Components

| Component             | Responsibility                                                                   |
| --------------------- | -------------------------------------------------------------------------------- |
| `TCPServer`           | Creates the listening socket, accepts clients and handles file-transfer requests |
| `TCPClient`           | Connects to the server and requests/downloads files                              |
| `ThreadPool`          | Manages worker threads and the shared task queue                                 |
| `server_main.cpp`     | Server application entry point                                                   |
| `main.cpp`            | Client application entry point                                                   |
| `test_threadpool.cpp` | Automated ThreadPool test                                                        |
| `results.csv`         | Concurrent-transfer benchmark results                                            |

---

## Requirements

* Linux environment
* C++17 compatible compiler
* CMake
* POSIX/Linux socket APIs
* pthread support

The project can be built and executed in a Linux environment such as **GitHub Codespaces**.

---

## Build Instructions

Clone the repository and enter the project directory:

```bash
git clone https://github.com/Prag-git-cmd/High_performance_multithreaded_file_transfer_server.git

cd High_performance_multithreaded_file_transfer_server
```

Create the build directory:

```bash
mkdir -p build
cd build
```

Configure the project:

```bash
cmake ..
```

Build the project:

```bash
cmake --build .
```

The build produces the server, client, and test executable.

---

## Running the Server

From the `build` directory:

```bash
./server
```

The server uses a default worker-thread configuration.

A custom number of worker threads can also be provided:

```bash
./server 3
```

For example:

```text
Starting server with 3 worker threads
Socket created successfully
Socket bound to port 8080
Server is listening...
```

---

## Running the Client

From the `build` directory, the client can request a file from the server.

Example:

```bash
./client download ../large_file.bin downloaded_test.bin
```

Example output:

```text
Connected to server
Downloaded 104857600 bytes
Transfer time: 0.806 seconds
Throughput: 124.069 MB/s
```

The exact throughput can vary depending on the execution environment and system load.

---

## Testing

The project uses **CTest** for automated testing.

After building:

```bash
ctest --output-on-failure
```

Current test coverage includes the ThreadPool implementation.

Example result:

```text
Test project .../build
    Start 1: ThreadPoolTest
1/1 Test #1: ThreadPoolTest ............ Passed

100% tests passed, 0 tests failed out of 1
```

---

## File Integrity Verification

Transferred files were validated using SHA-256 checksums and direct file comparison during testing.

Example:

```bash
sha256sum large_file.bin downloaded_test.bin
```

Matching SHA-256 hashes confirm that the transferred file contents are identical.

The files were also validated using:

```bash
cmp large_file.bin downloaded_test.bin
```

This provides an additional check that the transferred data matches the original file.

---

## Performance Benchmarking

The project includes concurrent file-transfer benchmark results in:

```text
docs/benchmarks/results.csv
```

The benchmark records the number of concurrent clients, total data transferred, total transfer time, and aggregate throughput.

### Recorded Results

| Clients | Total Data (MB) | Total Time (s) | Aggregate Throughput (MB/s) |
| ------: | --------------: | -------------: | --------------------------: |
|       1 |             100 |           0.76 |                      131.58 |
|       3 |             300 |           1.98 |                      151.52 |
|       5 |             500 |           4.12 |                      121.36 |
|       4 |             400 |          1.641 |                      243.75 |

These measurements are environment-dependent and are intended primarily to demonstrate the server's concurrent-transfer behavior.

---

## Concurrent Client Testing

The server was tested with multiple simultaneous clients transferring the same large test file.

The tests demonstrated that:

* Multiple clients can be served concurrently.
* Work is distributed among the configured worker threads.
* The server continues processing transfers while other workers handle additional clients.
* Downloaded files were verified against the original file using SHA-256 and file comparison.
* Aggregate throughput varies with the number of concurrent clients and the execution environment.

---

## Technical Highlights

### Thread-safe producer-consumer model

The server uses a shared task queue protected by synchronization primitives.

The producer side accepts client connections and adds work to the queue, while worker threads consume and execute queued tasks.

### Condition variables

Worker threads wait efficiently when there is no available work instead of continuously polling the queue.

### Fixed-size thread pool

The number of worker threads is configurable, providing controlled concurrency without creating an unbounded number of threads.

### Linux system programming

The project uses Linux/POSIX functionality including:

* TCP sockets
* File descriptors
* File I/O
* Threads
* Mutexes
* Condition variables
* `sendfile()`

### Zero-copy-oriented file transfer

Linux `sendfile()` is used as an alternative file-transfer mechanism to reduce unnecessary user-space buffering and copying.

### CMake and CTest

The project uses CMake for build configuration and CTest for automated test execution.

---

## Performance Considerations

The project explores several systems-level performance considerations:

* Thread creation overhead
* Worker-thread reuse
* Synchronization overhead
* File I/O
* Socket I/O
* Buffered versus `sendfile()` transfer
* Concurrent client handling
* Aggregate throughput

The benchmark results provide a basis for comparing server behavior under different client loads.

---

## Technologies Used

**Programming Language**

* C++17

**Operating System / APIs**

* Linux
* POSIX APIs
* TCP/IP sockets
* pthreads
* Linux `sendfile()`

**Concurrency**

* Multithreading
* Thread pool
* Mutex
* Condition variable
* Thread-safe task queue

**Build & Testing**

* CMake
* CTest

**Benchmarking & Validation**

* CSV benchmark data
* SHA-256
* `cmp`

---

## Future Improvements

Potential future extensions include:

* Transfer rate limiting
* Connection timeout handling
* More comprehensive request/response protocol
* Graceful server shutdown and signal handling
* Per-client performance metrics
* Structured logging
* Improved error recovery and retry mechanisms
* Automated benchmark execution
* CPU and memory profiling
* Linux `epoll`-based event handling
* TLS-secured file transfer

These are potential extensions and are not currently part of the implemented feature set.

---

## Learning Outcomes

This project provided practical experience with:

* Linux systems programming
* C++ multithreading
* Thread-pool architecture
* TCP socket programming
* Synchronization primitives
* Producer-consumer design
* File descriptor based I/O
* Linux `sendfile()`
* CMake-based project organization
* Automated testing with CTest
* Concurrent performance benchmarking
* Debugging and validating multithreaded applications

---

## Author

**Pragyanshree Sahoo**

Embedded Linux / C++ Software Engineering

---

## License

This project is intended for systems programming, learning, experimentation, and portfolio purposes.
