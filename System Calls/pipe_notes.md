# Understanding pipe() in Operating Systems (C++)

## What is a Pipe?
A **pipe** is a unidirectional data channel provided by the kernel. 
It connects two file descriptors:
- `fd[0]` → read end
- `fd[1]` → write end

If one process writes data into `fd[1]`, another process can read that same data from `fd[0]`.
Pipes exist only in memory (inside the kernel), not on disk.

---

## Example Code (Parent-Child Communication)

```cpp
#include <iostream>
#include <unistd.h>
#include <cstring>

int main() {
    int fd[2]; // fd[0] = read end, fd[1] = write end

    if (pipe(fd) == -1) {
        std::cerr << "Pipe creation failed\n";
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Fork failed\n";
        return 1;
    }

    if (pid == 0) {
        // ----- CHILD PROCESS -----
        close(fd[1]);  // Close write end (not needed)
        char buffer[100];
        ssize_t bytesRead = read(fd[0], buffer, sizeof(buffer) - 1);
        buffer[bytesRead] = '\0';
        std::cout << "Child received: " << buffer << std::endl;
        close(fd[0]);
    } else {
        // ----- PARENT PROCESS -----
        close(fd[0]);  // Close read end (not needed)
        const char* msg = "Hello from the parent process!";
        write(fd[1], msg, strlen(msg));
        close(fd[1]);
    }
    return 0;
}
```

---

## How It Works Step-by-Step

1. `pipe(fd)` → kernel creates a pipe (a memory buffer) and returns two descriptors.
2. `fork()` → duplicates the process, so both parent and child share the same pipe.
3. Parent closes the read end, writes into `fd[1]`.
4. Child closes the write end, reads from `fd[0]`.
5. The kernel transfers the data internally between both processes.

Output:
```
Child received: Hello from the parent process!
```

---

## What Happens If the Child Runs First?

If the **child process** runs before the parent writes anything:
- The child calls `read(fd[0])`.
- The kernel sees the pipe is empty and **blocks** the child (puts it to sleep).
- When the parent writes into `fd[1]`, the kernel wakes up the child.
- The child resumes, reads the data, and continues execution.

This is called **blocking I/O** — the process waits until data becomes available.

---

## Why No Deadlock?

Each process only closes its **own copy** of the file descriptors:
- When the child closes `fd[1]`, the parent’s write end is still open.
- When the parent closes `fd[0]`, the child’s read end is still open.

The kernel only destroys an end when *all* processes have closed that end.
Therefore, as long as one process keeps a valid write end, the pipe is alive.

Deadlock happens only if **both** processes are waiting for data but **neither writes**.

---

## Summary

| Operation | Process | Description |
|------------|----------|-------------|
| `pipe(fd)` | Kernel | Creates shared pipe |
| `fork()` | Kernel | Duplicates file descriptors for both processes |
| `write(fd[1], msg)` | Parent | Writes to pipe buffer |
| `read(fd[0], buffer)` | Child | Reads from pipe buffer |
| Close unused ends | Both | Prevents indefinite blocking |

---

### Key Concepts
- Pipes are **one-way** communication channels.
- Reading before writing blocks the reader.
- `read()` returns 0 (EOF) when all write ends are closed.
- Used heavily in shells and inter-process communication (`ls | wc -l`).

