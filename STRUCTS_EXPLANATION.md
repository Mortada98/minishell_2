# Minishell Pipeline Structures Documentation

This document explains the key structures used in the minishell pipeline execution system with visual diagrams.

## 📋 Overview

The minishell uses four main structures to manage pipeline execution:

1. **`t_pipeline_state`** - Tracks the current state of pipeline execution
2. **`t_parent_params`** - Parameters for parent process operations
3. **`t_child_params`** - Parameters for child process operations
4. **`t_exec_params`** - Execution parameters shared between processes

---

## 🔄 t_pipeline_state

```c
typedef struct s_pipeline_state
{
    int prev_fd;      // Previous command's output file descriptor
    int pid_count;    // Number of child processes created
    int fd_out;       // Current output file descriptor
    int fd_in;        // Current input file descriptor
} t_pipeline_state;
```

### 📊 Visual Representation

```
┌─────────────────────────────────────┐
│          t_pipeline_state           │
├─────────────────────────────────────┤
│ prev_fd    │ Pipe from previous cmd │
│ pid_count  │ Number of children     │
│ fd_out     │ Current output FD      │
│ fd_in      │ Current input FD       │
└─────────────────────────────────────┘
```

### 🎯 Purpose
- **State Management**: Keeps track of the current pipeline execution state
- **File Descriptor Chain**: Maintains the chain of file descriptors between commands
- **Process Counting**: Tracks how many child processes have been spawned

### 🔗 Flow Example
```
cmd1 | cmd2 | cmd3

State Evolution:
┌─────────┬─────────┬─────────┬─────────┐
│ Command │ prev_fd │ fd_out  │ fd_in   │
├─────────┼─────────┼─────────┼─────────┤
│ cmd1    │   -1    │  pipe1  │   0     │
│ cmd2    │ pipe1   │  pipe2  │ pipe1   │
│ cmd3    │ pipe2   │   1     │ pipe2   │
└─────────┴─────────┴─────────┴─────────┘
```

---

## 👨‍👦 t_parent_params

```c
typedef struct s_parent_params
{
    int     *prev_fd;     // Pointer to previous file descriptor
    int     *fd;          // Pointer to current pipe file descriptors
    pid_t   *pids;        // Array of child process IDs
    int     *pid_count;   // Pointer to process counter
} t_parent_params;
```

### 📊 Visual Representation

```
┌─────────────────────────────────────┐
│          t_parent_params            │
├─────────────────────────────────────┤
│ prev_fd  → │ Points to previous FD  │
│ fd       → │ Points to pipe array   │
│ pids     → │ Array of child PIDs    │
│ pid_count→ │ Points to PID counter  │
└─────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────┐
│           Memory Layout             │
├─────────────────────────────────────┤
│ pids[0] = 1001  (child 1)          │
│ pids[1] = 1002  (child 2)          │
│ pids[2] = 1003  (child 3)          │
│ ...                                 │
└─────────────────────────────────────┘
```

### 🎯 Purpose
- **Process Management**: Parent keeps track of all child processes
- **Resource Cleanup**: Enables proper cleanup of file descriptors and processes
- **Synchronization**: Allows parent to wait for all children to complete

### 🔄 Parent Process Flow
```
Parent Process Workflow:
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│ Fork Child 1 │───▶│ Store PID[0] │───▶│ Setup Pipes  │
└──────────────┘    └──────────────┘    └──────────────┘
        │                   │                   │
        ▼                   ▼                   ▼
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│ Fork Child 2 │───▶│ Store PID[1] │───▶│ Wait for All │
└──────────────┘    └──────────────┘    └──────────────┘
```

---

## 👶 t_child_params

```c
typedef struct s_child_params
{
    t_command   *cmd;     // Pointer to command structure
    t_command   *curr;    // Current command being processed
    t_data      **data;   // Pointer to shell data
    char        ***env;   // Pointer to environment variables
} t_child_params;
```

### 📊 Visual Representation

```
┌─────────────────────────────────────┐
│          t_child_params             │
├─────────────────────────────────────┤
│ cmd    → │ First command in chain   │
│ curr   → │ Current command          │
│ data   → │ Shell data structure     │
│ env    → │ Environment variables    │
└─────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────┐
│          Command Chain              │
├─────────────────────────────────────┤
│ cmd1 → cmd2 → cmd3 → NULL           │
│  ▲                                  │
│  │                                  │
│ curr (points to current command)    │
└─────────────────────────────────────┘
```

### 🎯 Purpose
- **Command Execution**: Provides child process with command to execute
- **Context Access**: Gives access to shell data and environment
- **Navigation**: Allows traversal through command pipeline

### 🔄 Child Process Flow
```
Child Process Workflow:
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│ Receive      │───▶│ Setup        │───▶│ Execute      │
│ Parameters   │    │ Redirections │    │ Command      │
└──────────────┘    └──────────────┘    └──────────────┘
        │                   │                   │
        ▼                   ▼                   ▼
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│ Close unused │───▶│ Dup2 FDs     │───▶│ execve()     │
│ file desc.   │    │              │    │              │
└──────────────┘    └──────────────┘    └──────────────┘
```

---

## ⚙️ t_exec_params

```c
typedef struct s_exec_params
{
    int prev_fd;        // Previous file descriptor
    int *fd;            // Current pipe file descriptors
    int *fd_out;        // Output file descriptor pointer
    int *fd_in;         // Input file descriptor pointer
    int save;           // Saved file descriptor
    int saved_stdin;    // Saved stdin file descriptor
} t_exec_params;
```

### 📊 Visual Representation

```
┌─────────────────────────────────────┐
│          t_exec_params              │
├─────────────────────────────────────┤
│ prev_fd      │ Previous pipe FD     │
│ fd        → │ Current pipe array    │
│ fd_out    → │ Output FD pointer     │
│ fd_in     → │ Input FD pointer      │
│ save         │ Backup FD             │
│ saved_stdin  │ Original stdin        │
└─────────────────────────────────────┘
```

### 🎯 Purpose
- **File Descriptor Management**: Manages all file descriptors for execution
- **State Preservation**: Saves original stdin for restoration
- **Pipe Coordination**: Coordinates pipe setup between processes

---

## 🔗 Structure Relationships

### 📊 Interaction Diagram

```
                    Pipeline Execution Flow
    ┌─────────────────────────────────────────────────────────┐
    │                    PARENT PROCESS                       │
    │                                                         │
    │  ┌─────────────────┐     ┌─────────────────┐           │
    │  │ t_pipeline_state│────▶│ t_parent_params │           │
    │  │                 │     │                 │           │
    │  │ • prev_fd       │     │ • pids[]        │           │
    │  │ • pid_count     │     │ • fd pointers   │           │
    │  │ • fd_out/in     │     │ • pid_count ptr │           │
    │  └─────────────────┘     └─────────────────┘           │
    │            │                       │                   │
    │            │                       │                   │
    │            ▼                       ▼                   │
    │  ┌─────────────────────────────────────────────────────┤
    │  │                 FORK()                              │
    │  └─────────────────────────────────────────────────────┤
    │                                                         │
    └─────────────────────────────────────────────────────────┘
                                │
                                ▼
    ┌─────────────────────────────────────────────────────────┐
    │                   CHILD PROCESS                         │
    │                                                         │
    │  ┌─────────────────┐     ┌─────────────────┐           │
    │  │ t_child_params  │────▶│ t_exec_params   │           │
    │  │                 │     │                 │           │
    │  │ • cmd           │     │ • fd management │           │
    │  │ • curr          │     │ • saved_stdin   │           │
    │  │ • data          │     │ • prev_fd       │           │
    │  │ • env           │     │                 │           │
    │  └─────────────────┘     └─────────────────┘           │
    │            │                       │                   │
    │            ▼                       ▼                   │
    │  ┌─────────────────────────────────────────────────────┤
    │  │              EXECUTE COMMAND                        │
    │  └─────────────────────────────────────────────────────┤
    └─────────────────────────────────────────────────────────┘
```

---

## 🚀 Usage Example

### Pipeline: `ls -l | grep txt | wc -l`

```
Step-by-Step Execution:

1. Initial State:
   ┌─────────────────┐
   │ pipeline_state  │
   │ prev_fd = -1    │
   │ pid_count = 0   │
   └─────────────────┘

2. Command 1 (ls -l):
   ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
   │ parent_params   │    │ child_params    │    │ exec_params     │
   │ pids[0] = 1001  │───▶│ cmd = "ls -l"   │───▶│ fd_out = pipe1  │
   │ fd = [3,4]      │    │ curr = cmd1     │    │ prev_fd = -1    │
   └─────────────────┘    └─────────────────┘    └─────────────────┘

3. Command 2 (grep txt):
   ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
   │ parent_params   │    │ child_params    │    │ exec_params     │
   │ pids[1] = 1002  │───▶│ cmd = "grep txt"│───▶│ fd_in = pipe1   │
   │ fd = [5,6]      │    │ curr = cmd2     │    │ fd_out = pipe2  │
   └─────────────────┘    └─────────────────┘    └─────────────────┘

4. Command 3 (wc -l):
   ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
   │ parent_params   │    │ child_params    │    │ exec_params     │
   │ pids[2] = 1003  │───▶│ cmd = "wc -l"   │───▶│ fd_in = pipe2   │
   │ pid_count = 3   │    │ curr = cmd3     │    │ fd_out = stdout │
   └─────────────────┘    └─────────────────┘    └─────────────────┘
```

---

## 🔧 Key Functions

### Structure Initialization
- `initialize_pipeline_state()` - Sets up initial pipeline state
- `setup_params_and_loop()` - Configures execution parameters
- `setup_params_helper()` - Helper for parameter setup

### Process Management
- `handle_parent_process()` - Parent process operations
- `execute_child_process()` - Child process execution
- `cleanup_pipeline_fds()` - File descriptor cleanup

### Resource Management
- `close_all_fds()` - Closes all file descriptors
- `close_fds_except_std()` - Preserves standard FDs
- `cleanup_and_exit()` - Complete cleanup on exit

---

## 💡 Best Practices

1. **Always initialize structures** before use
2. **Proper cleanup** of file descriptors and processes
3. **Error handling** at each step
4. **Memory management** with garbage collector
5. **Signal handling** for proper termination

This modular design allows for clean separation of concerns and efficient pipeline execution in the minishell.
