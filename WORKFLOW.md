# Workflow Diagrams

## Overview

This document outlines the process flows for the atomic brokerless discovery library. There are two separate workflows based on kernel capabilities:

- **Brokerless discovery** (Ubuntu 24.04, kernel ≥5.6) using `pidfd_open` and `pidfd_getfd`.
- **Fallback handshake** (Ubuntu 20.04, kernel 5.4) using Unix domain sockets and `SCM_RIGHTS`.

### Common Steps

1. **Publication** – A publisher process atomically publishes service metadata via `O_TMPFILE` and `linkat` to a directory under `/run`. Metadata includes the process ID, supported capabilities, and an optional UDS path.
2. **Discovery** – A consumer enumerates available services via directory scanning or `inotify`. For each service, it reads the metadata and decides which acquisition path to use based on kernel features.

### Workflow 1: Brokerless (pidfd_getfd)

When the kernel supports `pidfd_getfd` (Ubuntu 24.04):

```
Publisher Process                          Consumer Process
-------------------                        ------------------
1. Create shared memory via memfd         1. Enumerate /run/ipc-bus
   and set up ring buffer                 2. Read metadata (pid, capabilities)
2. Create eventfd for notifications       3. pidfd = pidfd_open(pid)
3. Publish metadata with linkat           4. memfd  = pidfd_getfd(pidfd, memfd_fd)
                                          5. evtfd  = pidfd_getfd(pidfd, eventfd)
                                          6. mmap shared memfd
                                          7. epoll/io_uring wait on eventfd
```

**Notes:**

- There is no handshake after publication; the publisher does not need to exchange sockets with the consumer.  
- The consumer duplicates file descriptors directly via the kernel and then maps the shared memory and waits on the eventfd.

### Workflow 2: Fallback (SCM_RIGHTS handshake)

On older kernels without `pidfd_getfd` (Ubuntu 20.04):

```
Publisher Process                          Consumer Process
-------------------                        ------------------
1. Create shared memory via memfd         1. Enumerate /run/ipc-bus
   and set up ring buffer                 2. Read metadata (includes UDS path)
2. Create eventfd for notifications       3. Connect to the UDS
3. Create and listen on a UDS             4. Receive memfd and eventfd via SCM_RIGHTS
4. Accept connection and send memfd       5. Close the UDS
   and eventfd via SCM_RIGHTS             6. mmap shared memfd
5. Publish metadata with linkat           7. epoll/io_uring wait on eventfd
```

**Notes:**

- The UDS handshake occurs only once per consumer to transfer the file descriptors.  
- After receiving the descriptors, both sides close the socket; the data plane uses shared memory and eventfd in a lock‑free manner.

## Considerations

- The publication directory should be cleaned up if the publisher exits unexpectedly. Using `O_TMPFILE` ensures the file is removed automatically on process exit until it is linked.
- Service enumeration can be event-driven using `inotify`. A fallback periodic scan may be used if `inotify` is unavailable.
- Both workflows rely on `eventfd` for efficient notification and `memfd_create` for creating shared memory segments.

## Future Enhancements

- Provide optional registry caching to avoid re-reading metadata on every lookup.  
- Support service removal notifications via `inotify` so consumers can clean up when a publisher exits.  
- Add integration tests verifying end-to-end functionality for both workflows across different kernel versions.
