# atomic-brokerless-discovery  

## Project Purpose  
This repository aims to develop a C++26 library that provides **atomic, brokerless discovery** of processes on a single Linux host.  The goal is to build a lightweight discovery layer that exposes shared-memory communication endpoints without relying on a central daemon.  Once discovery is complete, processes can exchange file descriptors for shared memory regions and event file descriptors (eventfd) to implement high‑performance, lock‑free ring buffers.  The library will support Linux kernels available on Ubuntu 20.04 and Ubuntu 24.04, handling differences in available primitives such as `pidfd_open` / `pidfd_getfd`.  

## Research Summary  
During the preliminary research we explored existing IPC libraries and kernel primitives to understand what is already available and what gaps need to be filled:  
- **Existing IPC stacks** such as *Iceoryx*, *Iceoryx2* and *Aeron* provide zero‑copy shared memory messaging, but their discovery mechanisms rely on central brokers or are tightly coupled to their own protocols.  
- **Linux primitives** such as `memfd_create`, `eventfd`, `SCM_RIGHTS` over Unix domain sockets, and, in newer kernels (>=5.6), `pidfd_open` and `pidfd_getfd`, enable processes to share memory and notifications safely without a broker.  
- **Atomic publication patterns** using `O_TMPFILE` and `linkat` allow a process to publish metadata to a well‑known directory in a crash‑safe manner.  Readers can discover services using directory enumeration or `inotify` without locks.  
- There is currently **no standalone library** that implements atomic brokerless discovery as a reusable component.  Iceoryx2 comes closest but is still a complete middleware.  As a result, building a small, composable discovery layer will fill an important gap.  
- Kernel features differ across Ubuntu versions: Ubuntu 20.04 (5.4 kernel) lacks `pidfd_getfd` and requires a fallback handshake over a Unix domain socket using `SCM_RIGHTS` to transfer file descriptors; Ubuntu 24.04 (6.8 kernel) supports `pidfd_getfd` and enables fully brokerless FD acquisition.  

## Project Plan  
The project will progress through several phases:  
1. **Define the discovery API** – Design a C++26 API that represents services, publishes and discovers them atomically, and abstracts over the underlying kernel mechanisms.  
2. **Implement atomic publication** – Use `O_TMPFILE` + `linkat` to publish service metadata (e.g., process ID, capabilities, optional UDS path) into a well‑known directory under `/run`.  Ensure that publications are crash‑safe and appear atomically to readers.  
3. **Implement discovery enumeration** – Provide wait‑free enumeration of published services via directory scans or `inotify`.  Expose functions to list available services and to wait for new ones.  
4. **Implement file descriptor acquisition** –  
   - On kernels with `pidfd_getfd` (Ubuntu 24.04), use `pidfd_open` and `pidfd_getfd` to duplicate the publisher’s shared‑memory and eventfd descriptors without any handshake.  
   - On older kernels (Ubuntu 20.04), fall back to a one‑time Unix domain socket handshake and use `SCM_RIGHTS` to transfer the descriptors.  
5. **Shared‑memory ring buffer** – Build a lock‑free ring buffer on top of `memfd`‑based shared memory with support for multiple producers/consumers.  Provide templated interfaces for typed messages.  
6. **Eventfd integration** – Use `eventfd` to notify consumers when new data is available.  Expose functions for producers to signal and for consumers to wait using `epoll` or `io_uring`.  
7. **Documentation and examples** – Write comprehensive documentation and examples that demonstrate how to use the library to publish, discover, and communicate via shared memory.  
8. **CI/CD setup** – Configure GitHub Actions to build and test the library on Docker images for Ubuntu 20.04 and Ubuntu 24.04 using the latest LLVM (clang) compiler version 22 and C++26 standards.  

## Testing Strategy  
To ensure reliability and correctness, the project will include thorough unit tests and integration tests:  
- **Unit tests** will cover individual components such as atomic publication, discovery enumeration, file descriptor acquisition, ring buffer operations, and eventfd signaling.  We will write tests to verify correct behavior under normal conditions and edge cases (e.g., abrupt process termination, concurrent publishers).  
- **Integration tests** will simulate real‑world scenarios where multiple producer and consumer processes publish themselves, discover peers, exchange FDs, and communicate over shared memory.  These tests will validate end‑to‑end functionality across different kernel configurations.  
- **Sanitizer support** – All tests will be compiled and run under LLVM 22 with sanitizers enabled:  
  - AddressSanitizer (ASan) to catch memory errors.  
  - UndefinedBehaviorSanitizer (UBSan) to detect undefined behavior.  
  - ThreadSanitizer (TSan) to identify data races.  
  - MemorySanitizer (MSan) and the new **realtime sanitizer** (as available in LLVM 22) to detect use of uninitialized memory and other concurrency bugs.  
  Each sanitizer will be integrated into the CI pipeline to catch issues early.  
- **Cross‑version testing** – Our CI configuration will build and test the library inside Docker containers for both Ubuntu 20.04 and Ubuntu 24.04.  This ensures that the fallback path using `SCM_RIGHTS` is tested on 20.04 while the brokerless `pidfd_getfd` path is verified on 24.04.  
- **C++26 compliance** – The codebase will adopt C++26 features where appropriate.  Tests will verify that the library builds cleanly under the latest LLVM 22 toolchain with `-std=c++26`.  

By following this plan, the repository will evolve into a robust library that empowers developers to build high‑performance, lock‑free shared memory communication systems without the overhead of a central broker.  Let’s get started!
