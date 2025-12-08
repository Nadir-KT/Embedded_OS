================================================================================
Process Management & IPC NOTES
================================================================================

--------------------------------------------------------------------------------
1. Processes, Forking, and Daemons
--------------------------------------------------------------------------------
In embedded Linux, you'll write programs that:

- Run in the background (daemons)
- Spawn child processes to do work
- Handle signals for lifecycle and coordination
- Must not crash the system if the programs fail

--------------------------------------------------------------------------------
1.1 What is a Process?
--------------------------------------------------------------------------------
A process is an instance of a program in execution. When you launch a program, the Linux kernel loads its code into memory, allocates resources, and begins to execute it. This running entity is a process.

When you launch a program (`./my_program`), the Linux kernel does the following:

- Loads Code: Reads the executable file from disk into memory.
- Allocates Resources: Creates a private virtual memory space and assigns file descriptors (for stdin, stdout, stderr, etc.), CPU time and other system resources.
- Assigns a PID: Assigns it a unique Process ID (PID) and scheduling context
- Sets State: Tracks the process's execution state (running, sleeping, stopped, zombie).

Key characteristics:

- Process ID (PID): Each process is assigned a unique integer for identification.
- Hierarchy / Parent PID (PPID): Every process has a parent, which is the process that created it; (except for the `init` process, PID 1, usually `systemd`), forming a process tree.
- Processes can create child processes (`fork`/`clone`) and replace themselves (`exec`).
- Isolation / Virtual Memory: Processes are isolated from each other, each having its own virtual address space. This isolation prevents a faulty process from crashing the entire system.
- Resources: Each process has its own set of resources, including file descriptors, memory, and CPU time.
- Communication: Processes can communicate with each other using specific IPC mechanisms.

Common Commands for Process Inspection:
-   `ps aux` or `ps -ef`: List all running processes with detailed information.
-   `pstree`: Display the process hierarchy as a tree. (If not installed, `ps -axjf` is a good alternative).
-   `top` or `htop`: Show a live, interactive view of system resource usage by processes.
-   `kill <PID>`: Send a signal to a process (e.g., `kill -9 1234` sends SIGKILL).

--------------------------------------------------------------------------------
4. Threads vs. Processes
--------------------------------------------------------------------------------
Processes are independent and have separate memory spaces. Communication between them requires IPC mechanisms.
Threads (lightweight processes) exist within a process and share the same memory space, file descriptors, and other resources. This makes communication between threads easier and faster but also means a faulty thread can crash the entire process.

| Feature             | Process                               | Thread                                          |
|---------------------|---------------------------------------|-------------------------------------------------|
| **Memory**          | Isolated, separate address spaces     | Shared address space                            |
| **Creation**        | Slower (`fork()`)                     | Faster (`pthread_create()`)                     |
| **Communication**   | Requires IPC (Pipes, Shm, etc.)       | Easy (via shared global variables)              |
| **Synchronization** | IPC-based (e.g., Semaphores)          | Must use Mutexes, Semaphores, etc.              |
| **Fault Isolation** | High (one crashing process is fine)   | Low (one bad thread can kill the whole process) |

When to use which?
-   Use multiple processes for security, robustness, and to leverage multiple CPU cores for completely separate tasks.
-   Use multiple threads for tasks that need to share data frequently and have low-latency communication (e.g., a web server handling multiple requests).

--------------------------------------------------------------------------------
1.2 Process States and Lifecycle
--------------------------------------------------------------------------------
Processes in Linux transition through various states during their lifecycle:

- Running (R): The process is either executing on a CPU or is ready to run and waiting in the scheduler's run queue.
- Sleeping (S/D): The process is waiting for an event or a resource.
    - Interruptible Sleep (S): Waiting for an event (e.g., I/O, timer). Can be woken up by signals (S/W Interrupts).
    - Uninterruptible Sleep (D): Waiting directly on hardware (e.g., disk I/O), cannot be interrupted (e.g., disk operations). High numbers of these can indicate hardware problems.
- Stopped (T): The process execution is paused, usually by a signal like `SIGSTOP` (e.g., from `Ctrl+Z`).
- Zombie (Z): The process has terminated, but its entry in the process table still exists because its parent has not yet retrieved its exit status (a process of "reaping"). Zombies don't consume CPU or memory but do use a slot in the process table.

Lifecycle: A process starts as Running, may sleep for resources, and ends as Zombie until reaped. 
- A process that exits remains a zombie until its parent calls `wait()`/`waitpid()` to collect status.
- Long-running embedded systems should monitor for zombies to avoid process table exhaustion.

--------------------------------------------------------------------------------
1.3 How Processes Are Created (fork)
--------------------------------------------------------------------------------

In Linux, every new process is created by forking an existing process.
fork() = "Clone yourself"
When a process calls fork():

Linux creates an exact copy of the process
Both processes continue running from the same point
The original = parent process
The copy = child process
fork() returns different values to parent and child so they can tell who they are

Typical pattern: `fork()` duplicates the calling process. After `fork()` both parent and child continue from the call; `fork()` returns `0` (`-1` if the fork failed) in the child and the child's PID in the parent.

Parent calls fork()
    ↓
Two processes now exist
    ↓
Child: fork() returns 0      Parent: fork() returns child's PID
    ↓                             ↓
Child does its work         Parent waits or continues

Important variants and caveats:
- `vfork()` may suspend the parent until the child calls `exec()` or `_exit()`; use with care.
- `clone()` allows fine-grained resource sharing (used by `pthread`, containers, etc.).

--------------------------------------------------------------------------------
1.4 Understanding fork()
--------------------------------------------------------------------------------
Goal: Let's write a simple program to see fork() in action.
Create a file: nano fork_demo.c
Paste this code:
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;
    
    printf("Before fork: PID = %d\n", getpid());
    
    pid = fork();
    
    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        return 1;
    }
    else if (pid == 0) {
        // Child process
        printf("CHILD: My PID = %d, Parent PID = %d\n", 
               getpid(), getppid());
        sleep(2);
        printf("CHILD: Exiting\n");
    }
    else {
        // Parent process
        printf("PARENT: My PID = %d, Child PID = %d\n", 
               getpid(), pid);
        sleep(3);
        printf("PARENT: Exiting\n");
    }
    
    return 0;
}
```

Compile and run:
```sh
gcc fork_demo.c -o fork_demo
./fork_demo
```

Expected output:
Before fork: PID = 1234
PARENT: My PID = 1234, Child PID = 1235
CHILD: My PID = 1235, Parent PID = 1234
CHILD: Exiting
PARENT: Exiting

Note on Execution Order: The order in which the parent and child processes are scheduled to run after a `fork()` is not guaranteed. In the example above, the parent prints first, but it could be the other way around. It depends on the system's scheduler.

📝 Questions to Test Understanding:

How many processes are running after the fork()?
Which process executes first after fork() - parent or child?
What happens if the parent exits before the child?

Note: scheduling order of parent/child is non-deterministic. If the parent exits first, the child will be reparented to init (`systemd`).

Try this modified version - change the parent's sleep to 1 second instead of 3:
sleep(1);  // in parent section
Run it again. What do you notice about the child's parent PID? (It will become 1, the PID of the `systemd` process, which "adopts" orphan processes).

What This Means for Embedded Linux
In embedded systems:

Your main application might fork workers to handle different sensors
A network daemon forks to handle each connection
Process isolation means one crashing component doesn't kill everything
You need to understand this to debug why processes become zombies

--------------------------------------------------------------------------------
1.5 fork() + exec() Pattern
--------------------------------------------------------------------------------

Usually after forking, the child process runs a different program using exec().
exec() = "Replace yourself with another program"
Common pattern in daemons/services: `fork()` to create a child, then `exec()` in the child to run another program. The parent can `wait()` or continue.

The `exec()` family of functions replaces the current process image with a new one. When a child process is forked, it often calls one of the `exec` functions to run a different program.

Fork to create a child
Child calls exec() to run a different program
Parent continues or waits for child

This is how your shell works! When you type ls:

Shell forks itself
Child exec's /bin/ls
Parent waits for child to finish

In multithreaded programs, avoid doing complex work between `fork()` and `exec()` — only call async-signal-safe functions.

--------------------------------------------------------------------------------
1.6 Waiting for Child Processes: `wait()` and `waitpid()`
--------------------------------------------------------------------------------

A parent process should always clean up its children to prevent zombie processes. This is done by waiting for the child to terminate and reading its exit status.

*   `wait(int *status)`: Suspends the calling process until one of its children terminates.
*   `waitpid(pid_t pid, int *status, int options)`: Provides more control, allowing the parent to wait for a specific child (`pid`) and to do so without blocking (`options = WNOHANG`).

**Example with `wait()`:**
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("CHILD: Exiting...\n");
        _exit(0);
    } else {
        printf("PARENT: Waiting for child...\n");
        wait(NULL);
        printf("PARENT: Child has terminated.\n");
    }
    return 0;
}
```

--------------------------------------------------------------------------------
3. Creating Processes: `fork()`, `exec()`, `wait()`
--------------------------------------------------------------------------------

### The `fork()` System Call
In Linux, new processes are created by duplicating an existing one. The `fork()` system call creates a new child process that is an almost exact copy of the parent.
-   The parent and child continue executing from the instruction immediately following the `fork()` call.
-   `fork()` return value distinguishes them:
    -   In the **parent**, `fork()` returns the PID of the newly created child.
    -   In the **child**, `fork()` returns `0`.
    -   If `fork()` fails, it returns `-1`.

#### `fork()` Demo
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        // --- Child Process ---
        printf("CHILD: I am the child. My PID is %d, my parent's PID is %d.\n", getpid(), getppid());
        sleep(2);
        printf("CHILD: Exiting with status 5.\n");
        exit(5);
    } else {
        // --- Parent Process ---
        printf("PARENT: I am the parent. My PID is %d, I created child %d.\n", getpid(), pid);

        int status;
        pid_t child_pid = wait(&status); // Wait for any child to terminate

        if (child_pid > 0) {
            if (WIFEXITED(status)) {
                printf("PARENT: Child %d exited with status %d.\n", child_pid, WEXITSTATUS(status));
            } else {
                printf("PARENT: Child %d terminated abnormally.\n", child_pid);
            }
        }
        printf("PARENT: Exiting.\n");
    }
    return 0;
}
```
**Compile and Run:** `gcc fork_wait_demo.c -o fork_wait_demo && ./fork_wait_demo`

### The `exec()` Family of Functions
The `exec()` functions **replace** the current process image with a new program. A common pattern is to `fork()` and then have the child call `exec()` to run a different application. This is how shells run commands.
-   `execlp("ls", "ls", "-l", NULL);` // `l`=list, `p`=path search
-   `execvp("ls", char_array_of_args);` // `v`=vector(array), `p`=path search

### Reaping Processes with `wait()` and `waitpid()`
A parent must clean up after its children. This is called "reaping".
-   `wait(&status)`: Blocks until *any* child process terminates.
-   `waitpid(pid, &status, options)`: Provides more control, allowing the parent to wait for a specific child (`pid`) or to check without blocking (`WNOHANG`).
-   The `status` integer contains information about *how* the child terminated, which can be inspected with macros like `WIFEXITED()` and `WEXITSTATUS()`. **This is crucial for robust error handling.**

### Orphan and Zombie Processes
-   **Zombie**: A child that has exited, but whose parent has not yet called `wait()` or `waitpid()`. It must be reaped.
-   **Orphan**: A child whose parent terminates before it does. Orphaned processes are "adopted" by the `init` process (PID 1), which automatically reaps them to prevent them from becoming zombies forever.

--------------------------------------------------------------------------------
1.7 Orphan Processes
--------------------------------------------------------------------------------
An orphan process is a child process whose parent has terminated before the child. In Linux, orphan processes are automatically adopted by the init process (PID 1, typically systemd). This prevents accumulation of zombies and ensures proper cleanup. If a parent exits without waiting for its children, the children become orphans and are reparented to init, which periodically reaps them.

--------------------------------------------------------------------------------
5.2 Process Termination
--------------------------------------------------------------------------------
Processes terminate via exit() or return from main(). Exit status (0-255) indicates success/failure. Parent uses wait() to retrieve status. In embedded systems, ensure proper cleanup to avoid resource leaks. Use _exit() in forked children to avoid flushing buffers twice.

--------------------------------------------------------------------------------
1.8 Zombie Processes and a Better fork() Pattern
--------------------------------------------------------------------------------

What happens if a child finishes before the parent is done?
A process that has completed execution but still has an entry in the process table. This occurs if the parent does not call `wait()` or `waitpid()`. The child process becomes a "zombie".
It's dead, but its entry in the process table still exists.
This is so the parent can still read its exit status.
Zombies don't use memory or CPU, but they do clog up the process table. If you create too many, you can't create new processes.
The parent process is responsible for "reaping" its children to clean up zombie processes. This is done with the `wait()` or `waitpid()` system calls.

Reaping strategies:

- Parent calls `wait()` or `waitpid()`.
- Use a `SIGCHLD` handler to call `waitpid(-1, &status, WNOHANG)` in a loop to reap multiple children.
- Use `signalfd()` in event-driven servers to handle child exit events safely in a single-threaded event loop.

Example safe `SIGCHLD` reaper (install with `sigaction`):
```c
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

void sigchld_handler(int signo) {
    int saved = errno;
    while (1) {
        pid_t pid;
        int status;
        pid = waitpid(-1, &status, WNOHANG);
        if (pid <= 0) break;
        /* inspect status if needed */
    }
    errno = saved;
}
```

Here is a better version of the program that uses `wait()`:
Create a file: fork_wait_demo.c
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> // <-- Required for wait()

int main() {
    pid_t pid;
    
    printf("Before fork: PID = %d\n", getpid());
    
    pid = fork();
    
    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        return 1;
    }
    else if (pid == 0) {
        // Child process
        printf("CHILD: My PID = %d, Parent PID = %d\n", 
               getpid(), getppid());
        sleep(2);
        printf("CHILD: Exiting\n");
    }
    else {
        // Parent process
        printf("PARENT: My PID = %d, Child PID = %d\n", 
               getpid(), pid);
        printf("PARENT: Waiting for child to exit...\n");
        wait(NULL); // <-- Parent waits for child to finish
        printf("PARENT: Child has exited. Exiting.\n");
    }
    
    return 0;
}
```

Compile and run:
```sh
gcc fork_wait_demo.c -o fork_wait_demo
./fork_wait_demo
```
With `wait()`, the parent will pause until the child has completed. This is a much safer and more robust way to manage child processes.

--------------------------------------------------------------------------------
1.9 Daemons: Background Processes
--------------------------------------------------------------------------------
A daemon is a process that runs in the background, without being attached to a controlling terminal.

Creating a Daemon:
The Modern Way (`systemd`): On most modern embedded Linux systems, you don't need to manually daemonize. You write a standard program and let the `systemd` service manager handle the daemonization, logging, and lifecycle management for you. This is the strongly recommended approach.

The Classic `fork-fork` Method:
1.  `fork()` the process and have the parent exit. The child is now orphaned and adopted by `init`.
2.  The child calls `setsid()` to start a new session, becoming the session leader and detaching from the controlling terminal.
3.  Optionally `fork()` again and have the new parent (the first child) exit. This ensures the daemon (the grandchild) is not a session leader and can never re-acquire a controlling terminal.
4.  Change working directory to `chdir("/")` (or safe location) to prevent holding a mount point open.
5.  Optionally, `umask(0)` if the daemon should have full control over file permissions.
6.  Close all file descriptors (`stdin`, `stdout`, `stderr`).

Example daemon code:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0); // Parent exits

    if (setsid() < 0) exit(1);

    pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0); // Child exits

    chdir("/");
    umask(0);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    daemonize();
    while (1) {
        // Daemon work
        sleep(1);
    }
    return 0;
}
```

On modern Linux systems, `systemd` is the preferred way to manage daemons. It simplifies daemon creation and management significantly.

To run your program as a service, create a **service file** in `/etc/systemd/system/`:

**`my_daemon.service`**
```ini
[Unit]
Description=My Awesome Daemon

[Service]
ExecStart=/usr/local/bin/my_daemon_program
Restart=always

[Install]
WantedBy=multi-user.target
```

Then, use `systemctl` to manage the service:
*   `sudo systemctl start my_daemon`
*   `sudo systemctl enable my_daemon` (start on boot)
*   `sudo systemctl status my_daemon`

--------------------------------------------------------------------------------
2. Signals and Signal Handling
--------------------------------------------------------------------------------

Signals are asynchronous software interrupts delivered to a process.

Common signals:
- `SIGINT` : Interrupt from keyboard (`Ctrl+C`).
- `SIGTERM`: Request graceful termination. Your application should handle this to shut down cleanly.
- `SIGKILL`: Forcefully terminate the process. This signal cannot be caught or ignored.
- `SIGHUP`: Hangup, sent when a controlling terminal is closed. Often used to tell daemons to reload their configuration.
- `SIGCHLD`: Sent to a parent when a child process terminates. Can be used to trigger reaping.
- SIGUSR1/SIGUSR2: User-defined signals

In embedded daemons, you MUST handle SIGTERM for graceful shutdown. 

Safe Signal Handling:
-   Use `sigaction()` instead of the older `signal()` for portable and reliable behavior.
-   Signal handlers must be simple and quick. Only call "async-signal-safe" functions inside them. Setting a `volatile sig_atomic_t` flag is the most common and safest pattern. Avoid calling functions like `printf` or `malloc` inside a handler.

Example `sigaction`:
```c
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

// `volatile` prevents the compiler from optimizing away checks on this variable.
// `sig_atomic_t` ensures that reads/writes are atomic operations.
volatile sig_atomic_t terminate_flag = 0;

void signal_handler(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        terminate_flag = 1;  // Set flag, don't exit directly
    }
    printf("Caught SIGTERM, shutting down gracefully...\n");
    _exit(0);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask); // Don't block any other signals during handling
    sa.sa_flags = 0;

    // Register handler for graceful shutdown signals
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    
    printf("Process %d running. Send SIGTERM or SIGINT to exit.\n", getpid());

    while (!terminate_flag) {
        // Do work
        sleep(1);
    }
    
    // Cleanup
    printf("Shutdown signal received. Cleaning up...\n");
    // Perform cleanup here (close files, release resources, etc.)
    printf("Exiting gracefully.\n");

    return 0;
}
```

--------------------------------------------------------------------------------
3. Process Scheduling
--------------------------------------------------------------------------------
Linux uses a scheduler to allocate CPU time. Key concepts:
- Priorities/Nice value: Priority adjustment hint to the scheduler (-20 highest, 19 lowest). Use nice() or renice.
- Real-time scheduling: For time-critical tasks, Linux offers real-time scheduling policies like `SCHED_FIFO` (First-In, First-Out) and `SCHED_RR` (Round-Robin).
- Commands: 
*   `nice -n <value> <program>`: Run a program with a specific nice value.
*   `renice -n <value> -p <pid>`: Change the nice value of a running process.
```sh
nice -n 10 ./program  // Run with lower priority
renice -n -5 -p <PID> // Increase priority of a running process
```
Real-time Scheduling: For time-critical tasks.
-   `SCHED_FIFO`: First-In, First-Out. Runs until it blocks, yields, or a higher priority task is ready.
-   `SCHED_RR`: Round-Robin. Like FIFO, but with a timeslice to ensure other tasks at the same priority level get a turn. Use for critical applications where latency matters.

In embedded systems, adjust priorities for critical processes.

Resource Limits: The `getrlimit()` and `setrlimit()` system calls allow a process to control its consumption of system resources, such as CPU time, memory, and open files.

--------------------------------------------------------------------------------
8. Inter-Process Communication (IPC)
--------------------------------------------------------------------------------

Processes can communicate with each other using various IPC mechanisms. POSIX IPC is generally preferred over the older System V IPC.
-   API Design: POSIX IPC uses file descriptors, so you can use standard `read()`, `write()`, and `close()`.
-   Naming: POSIX objects exist in the filesystem (e.g., `/dev/shm/my_shm`), making them easy to list (`ls`) and manage permissions (`chmod`). System V uses integer keys which can be difficult to track and can collide.

Here is a summary of common IPC mechanisms, from simplest to most complex.

1. PIPES (unnamed pipes | and named pipes FIFO)
   - Anonymous Pipes (`pipe()`): Simple one-way communication between related processes (parent/child). 
   - Process A writes → Process B reads
   -   `ls | grep "foo"` is a shell example.

   Example (shell):
   ```sh
   $ cat /tmp/data | grep "hello"
   ```
   
   C example:
   ```c
   #include <unistd.h>
   #include <stdio.h>
   #include <string.h>

   int main() {
       int pipefd[2];
       char buf[100];
       pipe(pipefd);
       if (fork() == 0) { // Child writes
           close(pipefd[0]);
           write(pipefd[1], "Hello", 5);
           close(pipefd[1]);
       } else { // Parent reads
           close(pipefd[1]);
           read(pipefd[0], buf, 5);
           printf("Received: %s\n", buf);
           close(pipefd[0]);
       }
       return 0;
   }
   ```

   - Named pipes (FIFO - `mkfifo()`): A special file in the filesystem. Unrelated processes can use it to communicate.
   mkfifo /tmp/myfifo; echo "data" > /tmp/myfifo

2. SOCKETS (TCP/UDP)
   -   Provide a bidirectional, connection-oriented communication channel, much like network sockets but entirely within the kernel.
   -   Very flexible and widely used for client-server applications on a single machine.
   - Also works for local process communication (Unix domain sockets)

3. MESSAGE QUEUES (System V and POSIX)
   -   A queue of messages. Processes can add and remove messages in a structured way.
   -   Messages have priorities. Useful for command/event passing.
   - System V: msgget, msgsnd, msgrcv (key-based)
   - POSIX: mq_open, mq_send, mq_receive (name-based, preferred)

4. SHARED MEMORY & SEMAPHORES (System V and POSIX)
   - Fastest IPC method, allowing multiple processes to share a region of memory, but requires careful synchronization mechanisms like semaphores.
   - Shared Memory: A region of memory is mapped into the address space of multiple processes. Data written by one is instantly visible to the others.
   - System V: shmget, shmat
   - POSIX: shm_open, mmap (preferred)
   - Race Conditions: If multiple processes write to shared memory at the same time, the data can become corrupted. This is a race condition.
   
   - Semaphores: A synchronization primitive used to protect shared resources. A semaphore acts as a counter. `sem_wait` decrements it (and blocks if it's zero), and `sem_post` increments it. A semaphore initialized to 1 is a mutex (mutual exclusion lock).
   - Synchronization primitives for controlling access to shared resources
   - Prevent race conditions in multi-process/multi-threaded environments
   - System V: semget, semop, semctl (key-based)
   - POSIX: sem_open, sem_wait, sem_post (name-based, preferred)
   - Example: Use semaphores with shared memory to ensure atomic operations

POSIX Shared Memory & Semaphore Example
This example has two programs: a "writer" that puts data into shared memory and a "reader" that consumes it. A semaphore is used to prevent the reader from reading before the writer is done.

**common.h:**
```c
#ifndef COMMON_H
#define COMMON_H

#define SHM_NAME "/my_shm"
#define SEM_NAME "/my_sem"
#define MSG_SIZE 256

#endif // COMMON_H
```

**shm_writer.c:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include "common.h"

int main() {
    // 1. Create shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, MSG_SIZE);

    // 2. Map shared memory into address space
    void *ptr = mmap(0, MSG_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // 3. Create a named semaphore, initialized to 0
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);

    // 4. Write to shared memory
    const char *message = "Hello from writer!";
    sprintf(ptr, "%s", message);
    printf("Writer: Wrote to shared memory.\n");

    // 5. "Post" to the semaphore (increment it), signaling the reader
    sem_post(sem);

    // 6. Clean up
    sleep(2); // Give reader a chance to read
    munmap(ptr, MSG_SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME);
    sem_close(sem);
    sem_unlink(SEM_NAME);

    return 0;
}
```

**shm_reader.c:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include "common.h"

int main() {
    sleep(1); // Wait for writer to start

    // 1. Open existing shared memory object
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    void *ptr = mmap(0, MSG_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

    // 2. Open existing semaphore
    sem_t *sem = sem_open(SEM_NAME, 0);

    // 3. "Wait" on the semaphore (blocks until it's > 0)
    printf("Reader: Waiting for semaphore...\n");
    sem_wait(sem);

    // 4. Read from shared memory
    printf("Reader: Semaphore acquired! Reading: \"%s\"\n", (char *)ptr);

    // 5. Clean up
    munmap(ptr, MSG_SIZE);
    close(shm_fd);
    sem_close(sem);

    return 0;
}
```
**Compile and Run:**
```bash
# In one terminal:
gcc shm_writer.c -o writer -lrt -lpthread
gcc shm_reader.c -o reader -lrt -lpthread

# In another terminal, start the reader first:
./reader

# Then in the first terminal, run the writer:
./writer
```
The reader will wait until the writer has finished writing and posts to the semaphore.

6. SIGNALS
   - Already covered above
   - Limited but fast

7. File Locking: `flock()` and `fcntl()` can be used to lock files for synchronization.

For embedded systems, prefer:
✓ PIPES for simple scripts
✓ POSIX IPC (message queues, shared memory, semaphores) over System V
✓ SOCKETS for complex systems
✓ Choose IPC based on latency, throughput, complexity and security.

================================================================================
END OF NOTES
================================================================================