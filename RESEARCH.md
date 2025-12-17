# Library Research  

This document summarizes the research conducted on existing IPC and service discovery libraries to evaluate whether they meet the requirements for an atomic brokerless discovery layer with shared‑memory ring buffers and eventfd notifications.  

## Iceoryx (classic)  
- **Purpose:** Zero‑copy shared‑memory IPC middleware for C++ and C.  
- **Discovery mechanism:** Central RouDi daemon using Unix domain sockets for service discovery; producers and consumers register with RouDi and get matched.  
- **Pros:** Mature, widely used in automotive and robotics; robust zero‑copy data plane.  
- **Cons:** Requires a central broker (RouDi); discovery tightly coupled to iceoryx protocol; heavyweight for a small discovery layer.  

## Iceoryx2  
- **Purpose:** Next‑generation evolution of iceoryx focusing on decentralized service discovery.  
- **Discovery mechanism:** Decentralized service‑based discovery; does not require a mandatory central broker; can integrate with Unix domain sockets or other discovery protocols.  
- **Pros:** Eliminates mandatory broker; modern APIs in C++ and Rust; lock‑free data plane.  
- **Cons:** Still part of a full middleware; discovery layer is not packaged as a standalone component.  

## Aeron  
- **Purpose:** High‑throughput messaging framework for IPC and networking.  
- **Discovery mechanism:** Uses a media driver process to coordinate streams between clients; producers and consumers connect through the media driver.  
- **Pros:** Very efficient for single‑host and network messaging; well tested in finance and trading systems.  
- **Cons:** Requires a media driver (central broker); oriented toward messaging semantics rather than simple discovery; heavy dependency for a lightweight discovery layer.  

## cpp‑ipc  
- **Purpose:** Lightweight C++ library providing shared‑memory ring buffers and notification primitives.  
- **Discovery mechanism:** None; it exposes primitives for shared memory and eventfd style signaling but does not provide a discovery mechanism.  
- **Pros:** Simple and lightweight; focuses on lock‑free shared memory communication.  
- **Cons:** Lacks any service discovery; users must implement their own handshake to exchange file descriptors.  

## Boost.Interprocess  
- **Purpose:** Generic library for shared memory, memory‑mapped files, and interprocess message queues.  
- **Discovery mechanism:** None; the library exposes primitives but leaves discovery to the application.  
- **Pros:** Portable, well maintained, flexible for various IPC patterns.  
- **Cons:** No built‑in service discovery; synchronization primitives may use locks or condition variables; not specialized for lock‑free ring buffers.  

## Other observations  
- Most existing libraries that provide zero‑copy shared memory IPC couple their discovery mechanisms to a central daemon (e.g., RouDi or media driver).  
- None of the surveyed libraries expose an atomic, brokerless discovery layer as a reusable component.  
- Modern Linux kernels (\u2265 5.6) provide the primitives needed to build such a layer: `O_TMPFILE` + `linkat()` for atomic publication, `memfd_create()` for shared memory, `eventfd` for notifications, `SCM_RIGHTS` for legacy file descriptor passing, and `pidfd_open()`/`pidfd_getfd()` for brokerless descriptor acquisition.  
- A small discovery library can fill this gap by publishing service metadata atomically to a well‑known directory and letting consumers discover services without locks.
