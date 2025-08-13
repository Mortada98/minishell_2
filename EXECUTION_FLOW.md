# Minishell Pipeline Execution Flow Visualization

This document provides a detailed visualization of how the pipeline execution flows through the various functions in the minishell, with comprehensive parameter tracking and file descriptor management.

## 🚀 Main Execution Flow with Parameter Details

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              EXECUTION FLOW                                │
└─────────────────────────────────────────────────────────────────────────────┘

                           📋 INPUT: Command Pipeline
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                          🎯 execute_command()                              │
│                                                                             │
│  Parameters:                                                               │
│  • t_command *cmd      - Linked list of commands                          │
│  • char ***env         - Environment variables                             │
│  • t_data **data       - Shell data structure                             │
│                                                                             │
│  Actions:                                                                  │
│  • fd.saved_stdin = dup(0)  [FD: 3] - Save original stdin                 │
│  • Check if single builtin command                                         │
│  • If single builtin → handle_single_builtin()                            │
│  • Else → Continue to pipeline execution                                   │
│  • Call empty_command() to validate                                        │
│  • Final cleanup: dup2(fd.saved_stdin, 0), close(fd.saved_stdin)          │
│                                                                             │
│  File Descriptors Status:                                                  │
│  • stdin (0) - saved as fd.saved_stdin [FD: 3]                            │
│  • stdout (1) - unchanged                                                  │
│  • stderr (2) - unchanged                                                  │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                      🔄 execute_pipeline_loop()                            │
│                                                                             │
│  Parameters:                                                               │
│  • t_command *cmd      - First command in pipeline                        │
│  • t_data **data       - Shell data                                        │
│  • char ***env         - Environment variables                             │
│  • int saved_stdin     - Original stdin FD [FD: 3]                        │
│                                                                             │
│  Actions:                                                                  │
│  • params.saved_stdin = saved_stdin [FD: 3]                               │
│  • Delegate to setup_params_and_loop()                                     │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                      ⚙️ setup_params_and_loop()                            │
│                                                                             │
│  Parameters:                                                               │
│  • t_command *cmd      - Command pipeline                                  │
│  • t_data **data       - Shell data                                        │
│  • char ***env         - Environment variables                             │
│  • t_exec_params *params - Execution parameters                           │
│                                                                             │
│  Actions & FD Management:                                                  │
│  • initialize_pipeline_state(params, &state)                              │
│  • pipe_fds[2] = {0, 0} - Initialize pipe array                           │
│  • params->fd = pipe_fds - Link pipe array to params                      │
│  • parent_params.prev_fd = &state.prev_fd                                 │
│  • parent_params.fd = params->fd                                           │
│  • pids = gc_calloc(count_pipeline_commands(cmd), sizeof(pid_t))          │
│  • parent_params.pids = pids                                               │
│  • parent_params.pid_count = &state.pid_count                             │
│  • child_params.cmd = cmd, .data = data, .env = env                       │
│                                                                             │
│  File Descriptors Status:                                                  │
│  • stdin (0) - still original                                              │
│  • params->saved_stdin [FD: 3] - preserved original stdin                 │
│  • pipe_fds[0], pipe_fds[1] - will be created per command                 │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                    🔧 initialize_pipeline_state()                          │
│                                                                             │
│  Parameters:                                                               │
│  • t_exec_params *params - Execution parameters                           │
│  • t_pipeline_state *state - Pipeline state                               │
│                                                                             │
│  Actions:                                                                  │
│  • state->prev_fd = -1        - No previous pipe initially               │
│  • state->pid_count = 0       - No children forked yet                   │
│  • state->fd_in = -1          - No input redirection initially           │
│  • state->fd_out = -1         - No output redirection initially          │
│  • params->prev_fd = state->prev_fd                                       │
│  • params->fd_out = &state->fd_out                                        │
│  • params->fd_in = &state->fd_in                                          │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                      🛠️ setup_params_helper()                               │
│                                                                             │
│  Parameters:                                                               │
│  • t_child_params *child_params - Child process parameters                │
│  • t_exec_params *params - Execution parameters                           │
│  • t_pipeline_state state - Current pipeline state                        │
│  • t_parent_params parent_params - Parent process parameters              │
│                                                                             │
│  Main Loop (for each command):                                             │
│  • curr = child_params->cmd                                                │
│  • while (curr && !curr->file)                                            │
│  • child_params->curr = curr                                               │
│  • Call process_pipeline_iteration()                                       │
│  • curr = curr->next                                                       │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                    🔄 process_pipeline_iteration()                         │
│                                                                             │
│  Parameters:                                                               │
│  • t_child_params *child_params - Child process parameters                │
│  • t_exec_params *params - Execution parameters                           │
│  • t_parent_params *parent_params - Parent process parameters             │
│                                                                             │
│  Actions & FD Changes:                                                     │
│  • params->prev_fd = *(parent_params->prev_fd)                            │
│  • params->save = dup(0) [FD: varies] - Save current stdin state          │
│  • setup_pipe(child_params->curr, params->fd)                             │
│  • pid = fork()                                                            │
│  • process_pipeline_helper(pid, parent_params, child_params, params)      │
│                                                                             │
│  File Descriptor Management:                                               │
│  • If last command: close(parent_params->fd[0]), close(parent_params->fd[1])│
│  • dup2(params->save, 0) - Restore stdin                                  │
│  • close(params->save) - Close saved FD                                    │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                        🔧 setup_pipe()                                     │
│                                                                             │
│  Parameters:                                                               │
│  • t_command *curr - Current command                                       │
│  • int *fd - Pipe file descriptor array                                    │
│                                                                             │
│  Actions:                                                                  │
│  • if (curr->next) - Only create pipe if there's a next command           │
│  •   pipe(fd) - Creates fd[0] (read) and fd[1] (write)                    │
│  •   Returns 1 on success, 0 on failure                                   │
│  • if (!curr->next) - Last command, no pipe needed                        │
│                                                                             │
│  File Descriptor Creation:                                                 │
│  • fd[0] = read end of pipe (e.g., FD: 4, 6, 8...)                       │
│  • fd[1] = write end of pipe (e.g., FD: 5, 7, 9...)                      │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                   🔄 process_pipeline_helper()                             │
│                                                                             │
│  Parameters:                                                               │
│  • int pid - Fork result                                                   │
│  • t_parent_params *parent_params - Parent parameters                     │
│  • t_child_params *child_params - Child parameters                        │
│  • t_exec_params *params - Execution parameters                           │
│                                                                             │
│  Fork Result Routing:                                                      │
│  • if (pid == 0) - Child process                                          │
│  •   handle_child_fork(child_params, params)                              │
│  • else if (pid > 0) - Parent process                                     │
│  •   handle_parent_process(pid, child_params->curr, parent_params)        │
│  • else (pid < 0) - Fork failed                                           │
│  •   handle_fork_error(params->save, params->saved_stdin)                 │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                ┌─────────┼─────────┐
                │         │         │
                ▼         ▼         ▼
        ┌───────────┐ ┌───────────┐ ┌───────────┐
        │ CHILD (0) │ │PARENT (>0)│ │ERROR (<0) │
        └───────────┘ └───────────┘ └───────────┘
                │         │         │
                ▼         ▼         ▼
┌─────────────────┐ ┌─────────────────┐ ┌─────────────────┐
│🧒handle_child   │ │👨handle_parent  │ │❌handle_fork    │
│  _fork()        │ │  _process()     │ │  _error()       │
└─────────────────┘ └─────────────────┘ └─────────────────┘
```

---

## 🧒 Child Process Path: handle_child_fork() - Detailed FD Management

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        🧒 handle_child_fork()                              │
│                                                                             │
│  Parameters:                                                               │
│  • t_child_params *child_params - Contains curr command, data, env         │
│  • t_exec_params *params - Contains FD management info                     │
│                                                                             │
│  Actions:                                                                  │
│  • Check if (child_params->cmd)->file == true                             │
│  • If file error: set_status(130), exit(130)                              │
│  • Else: execute_child_process(curr, data, params, env)                    │
│                                                                             │
│  File Descriptors Inherited:                                               │
│  • stdin (0) - current state                                               │
│  • stdout (1) - current state                                              │
│  • stderr (2) - unchanged                                                  │
│  • params->fd[0] - pipe read end (if exists)                              │
│  • params->fd[1] - pipe write end (if exists)                             │
│  • params->prev_fd - previous pipe read end                               │
│                                                                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                    🏃 execute_child_process()                              │
│                                                                             │
│  Parameters:                                                               │
│  • t_command *curr - Current command to execute                           │
│  • t_data **data - Shell data                                              │
│  • t_exec_params *params - Execution parameters with FDs                  │
│  • char ***env - Environment variables                                     │
│                                                                             │
│  Signal Setup:                                                             │
│  • signal(SIGQUIT, SIG_DFL) - Reset to default behavior                   │
│  • signal(SIGINT, SIG_DFL) - Reset to default behavior                    │
│                                                                             │
│  File Descriptor Setup - setup_child_io():                                │
│  • If curr->next exists (not last command):                               │
│    - dup2(fd[1], 1) - Redirect stdout to pipe write end                   │
│    - close(fd[1]) - Close original pipe write FD                          │
│    - close(fd[0]) - Close pipe read end (not needed by this child)        │
│  • If prev_fd != -1 (not first command):                                  │
│    - dup2(prev_fd, 0) - Redirect stdin from previous pipe                 │
│    - close(prev_fd) - Close original pipe read FD                         │
│                                                                             │
│  Redirection Handling:                                                     │
│  • excute_redirection_of_child(&curr, data, params, *env)                 │
│  • If redirection fails:                                                   │
│    - close_all_fds() - Close FDs 3 to system max                          │
│    - free_2d_array(*env) - Free environment                               │
│    - gc_cleanup() - Garbage collection                                     │
│    - exit(1) - Exit with error                                             │
│                                                                             │
│  Command Execution:                                                        │
│  • execute_command_logic(curr, &child_params, params, &bp)                │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

Child I/O Setup Detailed Visualization:
┌─────────────────────────────────────────────────────────────────────────────┐
│                           Child I/O Setup Details                          │
│                                                                             │
│  Pipeline: cmd1 | cmd2 | cmd3                                              │
│                                                                             │
│  Initial FD State (before any redirection):                                │
│  ┌─────────────────────────────────────────────────────────────────────────┐
│  │ FD 0: stdin                                                             │
│  │ FD 1: stdout                                                            │
│  │ FD 2: stderr                                                            │
│  │ FD 3: saved_stdin (parent)                                              │
│  │ FD 4: pipe1_read   (for cmd1 | cmd2)                                   │
│  │ FD 5: pipe1_write  (for cmd1 | cmd2)                                   │
│  │ FD 6: pipe2_read   (for cmd2 | cmd3)                                   │
│  │ FD 7: pipe2_write  (for cmd2 | cmd3)                                   │
│  └─────────────────────────────────────────────────────────────────────────┘
│                                                                             │
│  Child 1 (cmd1):                                                           │
│  ┌─────────────────────────────────────────────────────────────────────────┐
│  │ • curr->next exists (cmd2) → stdout redirection needed                 │
│  │ • dup2(fd[1], 1) → dup2(5, 1) - stdout now points to pipe1_write      │
│  │ • close(5) - close original pipe1_write FD                             │
│  │ • close(4) - close pipe1_read (not needed by cmd1)                     │
│  │ • prev_fd == -1 → no stdin redirection                                 │
│  │                                                                         │
│  │ Result: stdin=0, stdout=pipe1_write, stderr=2                          │
│  └─────────────────────────────────────────────────────────────────────────┘
│                                                                             │
│  Child 2 (cmd2):                                                           │
│  ┌─────────────────────────────────────────────────────────────────────────┐
│  │ • curr->next exists (cmd3) → stdout redirection needed                 │
│  │ • dup2(fd[1], 1) → dup2(7, 1) - stdout now points to pipe2_write      │
│  │ • close(7) - close original pipe2_write FD                             │
│  │ • close(6) - close pipe2_read (not needed by cmd2)                     │
│  │ • prev_fd == 4 (pipe1_read) → stdin redirection needed                 │
│  │ • dup2(prev_fd, 0) → dup2(4, 0) - stdin now points to pipe1_read      │
│  │ • close(4) - close original pipe1_read FD                              │
│  │                                                                         │
│  │ Result: stdin=pipe1_read, stdout=pipe2_write, stderr=2                 │
│  └─────────────────────────────────────────────────────────────────────────┘
│                                                                             │
│  Child 3 (cmd3):                                                           │
│  ┌─────────────────────────────────────────────────────────────────────────┐
│  │ • curr->next == NULL → no stdout redirection                           │
│  │ • prev_fd == 6 (pipe2_read) → stdin redirection needed                 │
│  │ • dup2(prev_fd, 0) → dup2(6, 0) - stdin now points to pipe2_read      │
│  │ • close(6) - close original pipe2_read FD                              │
│  │                                                                         │
│  │ Result: stdin=pipe2_read, stdout=1 (terminal), stderr=2                │
│  └─────────────────────────────────────────────────────────────────────────┘
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 👨 Parent Process Path: handle_parent_process() - FD Management

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       👨 handle_parent_process()                           │
│                                                                             │
│  Parameters:                                                               │
│  • pid_t pid - Child process ID                                            │
│  • t_command *curr - Current command being processed                       │
│  • t_parent_params *parent_params - Parent process parameters              │
│                                                                             │
│  File Descriptor Management:                                               │
│  • if (*(parent_params->prev_fd) != -1)                                   │
│    - close(*(parent_params->prev_fd)) - Close previous pipe read end       │
│    - WHY: Parent no longer needs to read from previous pipe                │
│                                                                             │
│  • if (curr->next) - If there's a next command in pipeline                │
│    - close(parent_params->fd[1]) - Close current pipe write end            │
│    - WHY: Parent doesn't write to pipe, only child does                    │
│    - *(parent_params->prev_fd) = parent_params->fd[0]                     │
│    - WHY: Save read end for next iteration                                 │
│                                                                             │
│  Process Management:                                                       │
│  • parent_params->pids[*(parent_params->pid_count)] = pid                  │
│    - Store child PID in array for later waiting                            │
│  • (*(parent_params->pid_count))++                                        │
│    - Increment counter of active children                                  │
│                                                                             │
│  IMPORTANT: NO waitpid() here - children run concurrently!                 │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

Parent State Evolution During Pipeline Creation:
┌─────────────────────────────────────────────────────────────────────────────┐
│                         Parent Process FD Management                       │
│                                                                             │
│  Pipeline: cmd1 | cmd2 | cmd3                                              │
│                                                                             │
│  After cmd1 fork:                                                          │
│  ┌─────────────────────────────────────────────────────────────────────────┐
│  │ • Created pipe1: fd[0]=4 (read), fd[1]=5 (write)                       │
│  │ • Child1 inherited both ends                                            │
│  │ • Parent closes fd[1]=5 (write end) - child writes, parent doesn't     │
│  │ • Parent keeps fd[0]=4 (read end) as prev_fd for next iteration        │
│  │ • pids[0] = 1001, pid_count = 1                                        │
│  │                                                                         │
│  │ FD State: prev_fd=4, pipe1_read=4 (saved for cmd2)                     │
│  └─────────────────────────────────────────────────────────────────────────┘
│                                                                             │
│  After cmd2 fork:                                                          │
│  ┌─────────────────────────────────────────────────────────────────────────┐
│  │ • Close prev_fd=4 (pipe1_read) - no longer needed by parent            │
│  │ • Created pipe2: fd[0]=6 (read), fd[1]=7 (write)                       │
│  │ • Child2 inherited pipe2 ends                                           │
│  │ • Parent closes fd[1]=7 (write end)                                     │
│  │ • Parent keeps fd[0]=6 (read end) as prev_fd for next iteration        │
│  │ • pids[1] = 1002, pid_count = 2                                        │
│  │                                                                         │
│  │ FD State: prev_fd=6, pipe2_read=6 (saved for cmd3)                     │
│  └─────────────────────────────────────────────────────────────────────────┘
│                                                                             │
│  After cmd3 fork:                                                          │
│  ┌─────────────────────────────────────────────────────────────────────────┐
│  │ • Close prev_fd=6 (pipe2_read) - no longer needed by parent            │
│  │ • No new pipe created (last command)                                    │
│  │ • pids[2] = 1003, pid_count = 3                                        │
│  │                                                                         │
│  │ FD State: prev_fd=-1, all pipes closed by parent                       │
│  └─────────────────────────────────────────────────────────────────────────┘
│                                                                             │
│  Why Parent Closes Pipe Ends:                                              │
│  ┌─────────────────────────────────────────────────────────────────────────┐
│  │ 1. Write ends: Parent never writes to pipes, only children do          │
│  │ 2. Read ends: Parent doesn't read pipe data, it's for next child       │
│  │ 3. Previous read ends: No longer needed once next child is forked      │
│  │ 4. Prevents resource leaks and ensures proper pipe closure             │
│  └─────────────────────────────────────────────────────────────────────────┘
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## ❌ Error Handling: handle_fork_error() - FD Cleanup

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        ❌ handle_fork_error()                              │
│                                                                             │
│  Parameters:                                                               │
│  • int save - Saved stdin FD from dup(0)                                   │
│  • int saved_stdin - Original stdin FD from execute_command()              │
│                                                                             │
│  Actions:                                                                  │
│  • perror("fork") - Print fork error message                               │
│  • close(save) - Close the saved current stdin                             │
│  • dup2(saved_stdin, 0) - Restore original stdin                          │
│  • close(saved_stdin) - Close the saved original stdin                     │
│                                                                             │
│  File Descriptor Recovery:                                                 │
│  • Ensures stdin is restored to original state                             │
│  • Prevents file descriptor leaks                                          │
│  • Returns control to caller with clean state                              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 🧹 Final Cleanup Phase: handle_core_dumped() - Process Synchronization

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     🧹 handle_core_dumped()                                │
│                                                                             │
│  Parameters:                                                               │
│  • int *pids - Array of child process IDs                                  │
│  • int pid_count - Number of child processes                               │
│  • t_data **data - Shell data                                              │
│                                                                             │
│  WHY NO waitpid() in handle_parent_process():                              │
│  • Parent must fork ALL children before waiting                            │
│  • Waiting during forking would break pipeline concurrency                 │
│  • This function handles ALL waiting after pipeline setup                  │
│                                                                             │
│  Actions:                                                                  │
│  • signal(SIGINT, SIG_IGN) - Ignore SIGINT during waiting                  │
│  • for (i = 0; i < pid_count; i++)                                        │
│    - waitpid(pids[i], &status, 0) - Wait for each child                   │
│    - Handle signals: SIGINT, SIGQUIT                                       │
│    - Set exit status from last command                                     │
│  • parent_status(status, flags, sig) - Final status processing            │
│                                                                             │
│  Signal Handling:                                                          │
│  • SIGINT (Ctrl+C): Set status 128+sig, print newline                     │
│  • SIGQUIT (Ctrl+\): Print "Quit (core dumped)"                           │
│  • Exit status: Use WEXITSTATUS of last child                             │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 🗑️ File Descriptor Cleanup: cleanup_pipeline_fds()

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                   🗑️ cleanup_pipeline_fds()                                │
│                                                                             │
│  Parameters:                                                               │
│  • t_exec_params *params - Execution parameters                           │
│  • t_pipeline_state *state - Pipeline state                               │
│                                                                             │
│  File Descriptor Cleanup Logic:                                            │
│  • if (state->fd_in > 2) close(state->fd_in)                              │
│    WHY: Only close if not stdin/stdout/stderr                             │
│  • if (state->fd_out > 2) close(state->fd_out)                            │
│    WHY: Only close if not stdin/stdout/stderr                             │
│  • if (params->fd[0] > 2) close(params->fd[0])                            │
│    WHY: Close last pipe read end if still open                            │
│  • if (params->fd[1] > 2) close(params->fd[1])                            │
│    WHY: Close last pipe write end if still open                           │
│  • if (state->prev_fd > 2) close(state->prev_fd)                          │
│    WHY: Close any remaining previous pipe                                  │
│                                                                             │
│  Why > 2 Check:                                                            │
│  • FD 0: stdin - never close                                               │
│  • FD 1: stdout - never close                                              │
│  • FD 2: stderr - never close                                              │
│  • FD > 2: Safe to close (pipes, files, etc.)                             │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 🔄 Complete Flow Example: `ls -l | grep txt | wc -l` - FD Tracking

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    COMPLETE EXECUTION FLOW WITH FD TRACKING                │
└─────────────────────────────────────────────────────────────────────────────┘

Initial State:
┌─────────────────────────────────────────────────────────────────────────────┐
│ FD 0: stdin (terminal)                                                     │
│ FD 1: stdout (terminal)                                                    │
│ FD 2: stderr (terminal)                                                    │
└─────────────────────────────────────────────────────────────────────────────┘

Step 1: execute_command()
┌─────────────────────────────────────────────────────────────────────────────┐
│ • fd.saved_stdin = dup(0) = 3                                              │
│                                                                             │
│ FD 0: stdin (terminal)                                                     │
│ FD 1: stdout (terminal)                                                    │
│ FD 2: stderr (terminal)                                                    │
│ FD 3: saved_stdin (copy of original stdin)                                 │
└─────────────────────────────────────────────────────────────────────────────┘

Step 2-4: Pipeline Setup (initialize_pipeline_state, setup_params_and_loop)
┌─────────────────────────────────────────────────────────────────────────────┐
│ • Allocate pids[3] for 3 commands                                          │
│ • Initialize state: prev_fd=-1, pid_count=0                                │
│ • params.saved_stdin = 3                                                   │
└─────────────────────────────────────────────────────────────────────────────┘

Command 1: "ls -l" Processing
┌─────────────────────────────────────────────────────────────────────────────┐
│ process_pipeline_iteration():                                              │
│ • params->save = dup(0) = 4 (save current stdin state)                    │
│                                                                             │
│ setup_pipe():                                                              │
│ • pipe(fd) creates: fd[0]=5 (read), fd[1]=6 (write)                       │
│                                                                             │
│ fork() = 1001:                                                             │
│                                                                             │
│ CHILD 1001:                                                                │
│ • handle_child_fork() → execute_child_process()                            │
│ • setup_child_io():                                                        │
│   - curr->next exists → dup2(6, 1) (stdout to pipe write)                 │
│   - close(6), close(5)                                                     │
│   - prev_fd == -1 → no stdin redirection                                  │
│ • Child FDs: stdin=0, stdout=pipe1_write, stderr=2                        │
│                                                                             │
│ PARENT:                                                                    │
│ • handle_parent_process():                                                 │
│   - prev_fd == -1 → nothing to close                                      │
│   - curr->next exists → close(6), prev_fd = 5                             │
│   - pids[0] = 1001, pid_count = 1                                         │
│ • Parent FDs: 0,1,2,3(saved),4(save),5(pipe1_read)                        │
│                                                                             │
│ process_pipeline_helper() cleanup:                                         │
│ • dup2(4, 0) (restore stdin), close(4)                                     │
│                                                                             │
│ Current FD State:                                                          │
│ FD 0: stdin (terminal)                                                     │
│ FD 1: stdout (terminal)                                                    │
│ FD 2: stderr (terminal)                                                    │
│ FD 3: saved_stdin                                                          │
│ FD 5: pipe1_read (saved as prev_fd)                                        │
└─────────────────────────────────────────────────────────────────────────────┘

Command 2: "grep txt" Processing
┌─────────────────────────────────────────────────────────────────────────────┐
│ process_pipeline_iteration():                                              │
│ • params->prev_fd = 5 (pipe1_read)                                         │
│ • params->save = dup(0) = 4                                                │
│                                                                             │
│ setup_pipe():                                                              │
│ • pipe(fd) creates: fd[0]=6 (read), fd[1]=7 (write)                       │
│                                                                             │
│ fork() = 1002:                                                             │
│                                                                             │
│ CHILD 1002:                                                                │
│ • setup_child_io():                                                        │
│   - curr->next exists → dup2(7, 1) (stdout to pipe2_write)                │
│   - close(7), close(6)                                                     │
│   - prev_fd == 5 → dup2(5, 0) (stdin from pipe1_read)                     │
│   - close(5)                                                               │
│ • Child FDs: stdin=pipe1_read, stdout=pipe2_write, stderr=2               │
│                                                                             │
│ PARENT:                                                                    │
│ • handle_parent_process():                                                 │
│   - prev_fd == 5 → close(5) (pipe1_read no longer needed)                 │
│   - curr->next exists → close(7), prev_fd = 6                             │
│   - pids[1] = 1002, pid_count = 2                                         │
│ • Parent FDs: 0,1,2,3(saved),4(save),6(pipe2_read)                        │
│                                                                             │
│ cleanup: dup2(4, 0), close(4)                                              │
│                                                                             │
│ Current FD State:                                                          │
│ FD 0: stdin (terminal)                                                     │
│ FD 1: stdout (terminal)                                                    │
│ FD 2: stderr (terminal)                                                    │
│ FD 3: saved_stdin                                                          │
│ FD 6: pipe2_read (saved as prev_fd)                                        │
└─────────────────────────────────────────────────────────────────────────────┘

Command 3: "wc -l" Processing
┌─────────────────────────────────────────────────────────────────────────────┐
│ process_pipeline_iteration():                                              │
│ • params->prev_fd = 6 (pipe2_read)                                         │
│ • params->save = dup(0) = 4                                                │
│                                                                             │
│ setup_pipe():                                                              │
│ • curr->next == NULL → no pipe created                                     │
│                                                                             │
│ fork() = 1003:                                                             │
│                                                                             │
│ CHILD 1003:                                                                │
│ • setup_child_io():                                                        │
│   - curr->next == NULL → no stdout redirection                            │
│   - prev_fd == 6 → dup2(6, 0) (stdin from pipe2_read)                     │
│   - close(6)                                                               │
│ • Child FDs: stdin=pipe2_read, stdout=1(terminal), stderr=2               │
│                                                                             │
│ PARENT:                                                                    │
│ • handle_parent_process():                                                 │
│   - prev_fd == 6 → close(6) (pipe2_read no longer needed)                 │
│   - curr->next == NULL → no new pipe                                      │
│   - pids[2] = 1003, pid_count = 3                                         │
│ • Parent FDs: 0,1,2,3(saved),4(save)                                      │
│                                                                             │
│ cleanup: dup2(4, 0), close(4)                                              │
│                                                                             │
│ Current FD State:                                                          │
│ FD 0: stdin (terminal)                                                     │
│ FD 1: stdout (terminal)                                                    │
│ FD 2: stderr (terminal)                                                    │
│ FD 3: saved_stdin                                                          │
└─────────────────────────────────────────────────────────────────────────────┘

Final Cleanup Phase:
┌─────────────────────────────────────────────────────────────────────────────┐
│ cleanup_pipeline_fds():                                                    │
│ • All pipes already closed by parent during execution                      │
│ • No additional FDs to clean                                               │
│                                                                             │
│ handle_core_dumped():                                                      │
│ • waitpid(1001, &status, 0) - wait for ls                                 │
│ • waitpid(1002, &status, 0) - wait for grep                               │
│ • waitpid(1003, &status, 0) - wait for wc                                 │
│ • Set final exit status from wc command                                    │
│                                                                             │
│ execute_command() final cleanup:                                           │
│ • dup2(3, 0) - restore original stdin                                      │
│ • close(3) - close saved_stdin                                             │
│                                                                             │
│ Final FD State:                                                            │
│ FD 0: stdin (terminal) - restored                                          │
│ FD 1: stdout (terminal)                                                    │
│ FD 2: stderr (terminal)                                                    │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 🔧 Function Responsibilities Summary with FD Management

| Function | Main Responsibility | FD Operations |
|----------|-------------------|---------------|
| `execute_command()` | Entry point, handle single builtins vs pipelines | `dup(0)→saved_stdin`, final `dup2(saved_stdin,0)` |
| `execute_pipeline_loop()` | Initialize exec_params and delegate to setup | Pass `saved_stdin` to params |
| `setup_params_and_loop()` | Setup all structures and coordinate execution | Initialize pipe arrays, manage cleanup |
| `initialize_pipeline_state()` | Initialize state variables to default values | Set all FD pointers to -1 |
| `setup_params_helper()` | Main command iteration loop | Coordinate FD passing between iterations |
| `process_pipeline_iteration()` | Handle single command: pipe, fork, execute | `dup(0)→save`, restore with `dup2(save,0)` |
| `setup_pipe()` | Create pipe between commands if needed | `pipe(fd)` creates read/write FD pair |
| `process_pipeline_helper()` | Route fork result to appropriate handler | Coordinate FD cleanup between parent/child |
| `handle_child_fork()` | Child process entry point | Inherit all FDs, setup I/O redirection |
| `handle_parent_process()` | Parent process cleanup and state management | Close write ends, save read ends, close prev FDs |
| `handle_fork_error()` | Error recovery when fork fails | Restore stdin, close saved FDs |
| `cleanup_pipeline_fds()` | Final FD cleanup | Close all remaining FDs > 2 |
| `handle_core_dumped()` | Process synchronization and waiting | No FD operations, focus on `waitpid()` |

---

## 🎯 Key Design Patterns with FD Management

### 1. **FD Lifecycle Management**
- **Creation**: Pipes created just before fork
- **Inheritance**: Children inherit all parent FDs  
- **Redirection**: Children redirect stdin/stdout using `dup2()`
- **Cleanup**: Parent closes unused ends immediately
- **Restoration**: Original stdin restored at each level

### 2. **Why Specific FDs are Closed When**

#### **Parent Process FD Closing Logic:**
```c
// After each fork in handle_parent_process():

if (prev_fd != -1) 
    close(prev_fd);      // Previous pipe read end no longer needed
    
if (curr->next) {
    close(fd[1]);        // Parent never writes to pipe
    prev_fd = fd[0];     // Save read end for next child
}
```

#### **Child Process FD Setup Logic:**
```c
// In setup_child_io():

if (curr->next) {
    dup2(fd[1], 1);      // Redirect stdout to pipe write end
    close(fd[1]);        // Close original FD after redirection
    close(fd[0]);        // Child doesn't need read end of own pipe
}

if (prev_fd != -1) {
    dup2(prev_fd, 0);    // Redirect stdin from previous pipe
    close(prev_fd);      // Close original FD after redirection
}
```

### 3. **Error Prevention Strategies**
- **FD > 2 checks**: Never close stdin/stdout/stderr
- **Immediate cleanup**: Close FDs as soon as they're no longer needed
- **State restoration**: Always restore original state on error
- **Resource tracking**: Use structures to track all open FDs

### 4. **Concurrency and Synchronization**
- **Fork phase**: Create all processes with proper FD setup
- **No early waiting**: Parent doesn't wait during fork phase
- **Batch waiting**: All `waitpid()` calls happen after pipeline setup
- **Signal handling**: Proper signal management during waiting

### 5. **Memory and Resource Management**
- **Garbage collector**: Automatic memory management
- **FD tracking**: Systematic tracking and cleanup
- **Process tracking**: PID array for proper waiting
- **State preservation**: Multiple levels of state saving/restoration

This comprehensive FD management ensures clean pipeline execution with no resource leaks and proper process coordination.
