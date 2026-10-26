# 🐚 Minishell

A command-line **Unix shell** implemented in C that reads commands, executes them as child processes, and manages them through a full **job control** system: foreground and background execution, stop and resume, and a live job table. It also supports **multi-stage pipelines** such as `ls | grep txt | wc`.

The project uses `fork()`, `execvp()`, `pipe()`, `dup2()` and `waitpid()` for process management, **POSIX signals** for `Ctrl+C` / `Ctrl+Z` handling, and a **singly linked list** to track every running or stopped job.

<p>
  <a href="https://github.com/JITH1"><img src="https://img.shields.io/badge/-GitHub-181717?style=for-the-badge&logo=github&logoColor=white" alt="GitHub"></a>
  <a href="https://www.linkedin.com/in/jithinjith"><img src="https://img.shields.io/badge/-LinkedIn-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white" alt="LinkedIn"></a>
</p>

---

## 🚀 Features

- ⚙️ Executes external commands using `fork()` + `execvp()`
- 🔗 Pipeline support with any number of stages (`cmd1 | cmd2 | cmd3 ...`) using `pipe()` and `dup2()`
- 📥 Loads the supported external command list from `ext_commands.txt` at startup
- 🏠 Built-in commands: `cd`, `pwd`, `exit`, `echo $$`, `echo $?`, `echo $SHELL`
- 🎨 Customizable prompt using `PS1=<text>`
- 🌈 Colored prompt and error messages using ANSI escape codes
- 🔙 Background execution using `&`
- 📋 Job listing with `jobs`, `jobs -l`, `jobs -r` and `jobs -s`
- ⏯️ Job control with `fg` and `bg`
- ⌨️ `Ctrl+C` terminates and `Ctrl+Z` stops the foreground job without exiting the shell
- 🧹 Automatic reaping of finished children (no zombie processes)
- 🔢 Exit status tracking with `echo $?`
- 🚫 Reports invalid commands, empty pipeline stages and invalid `cd` usage
- 🛠️ Makefile support for compilation

---

## 🛠️ Technologies & Core Concepts

### Language
- C

### Core Concepts
- Process Creation (`fork`, `exec`, `wait`)
- Inter-Process Communication (`pipe`, `dup2`)
- File Descriptor Management
- Signals and Signal Handling (`SIGINT`, `SIGTSTP`, `SIGCONT`, `SIGSTOP`, `SIGKILL`)
- Job Control (foreground / background / stopped)
- Singly Linked Lists
- Structures and Pointers
- Dynamic Memory Allocation
- File Handling
- String Parsing and Tokenizing (`strtok`)
- Exit Status Handling (`WIFEXITED`, `WIFSIGNALED`, `WIFSTOPPED`)
- Modular Programming
- Enumerations and Macros
- Makefile

### Development Tools
- GCC
- GNU Make
- Linux

---

## 🧱 Data Structure

Every process launched by the shell is tracked as a node in a **singly linked list** called the job table.

### Job Node (`Job`)

```c
typedef struct node
{
    int job_id;
    pid_t job_pid;
    State job_state;
    Type job_type;
    char command[100];
    struct node *link;
} Job;
```

| Field | Description |
|-------|-------------|
| `job_id` | Sequential job number shown to the user |
| `job_pid` | Process ID of the child |
| `job_state` | `RUNNING` or `STOP` |
| `job_type` | `FG` (foreground) or `BG` (background) |
| `command` | The command string that started the job |
| `link` | Next job in the list |

### Enumerations

```c
typedef enum { STOP, RUNNING } State;
typedef enum { FG, BG } Type;
```

### Structure Diagram

```
Head
 │
 ▼
┌────────────────┐    ┌────────────────┐    ┌────────────────┐
│ id    : 1      │    │ id    : 2      │    │ id    : 3      │
│ pid   : 4821   │───►│ pid   : 4830   │───►│ pid   : 4842   │───► NULL
│ state : RUNNING│    │ state : STOP   │    │ state : RUNNING│
│ type  : BG     │    │ type  : BG     │    │ type  : FG     │
│ cmd   : sleep  │    │ cmd   : sleep  │    │ cmd   : ls     │
└────────────────┘    └────────────────┘    └────────────────┘
```

- New jobs are appended at the tail of the list.
- A job is removed as soon as it exits or is killed.
- The table holds up to **100** jobs (`MAX_JOBS`).

### Pipeline Design

For a pipeline of `N` commands the shell creates `N - 1` pipes and forks `N` children. Each child connects its standard input and output to the neighbouring pipes with `dup2()`, closes every pipe end, and then calls `execvp()`.

```
  ls            grep txt            wc
┌──────┐      ┌──────────┐      ┌──────┐
│ cmd0 │─────►│   cmd1   │─────►│ cmd2 │
└──────┘ fd0  └──────────┘ fd1  └──────┘
 stdout  [1]→[0] stdin/stdout  [1]→[0]  stdin
```

- Command `i` reads from `fd[i-1][0]` (if `i > 0`)
- Command `i` writes to `fd[i][1]` (if `i < N - 1`)
- The parent closes all pipe ends after forking so that every reader receives EOF

### Shell Flow

```
          ┌──────────────────────────┐
          │ Reap finished children   │◄─────────────┐
          └────────────┬─────────────┘              │
                       ▼                            │
          ┌──────────────────────────┐              │
          │ Print prompt, read input │              │
          └────────────┬─────────────┘              │
                       ▼                            │
     ┌─────────────────┼──────────────────┐         │
     ▼                 ▼                  ▼         │
 ends with '&'?   contains '|' ?     PS1 / jobs /   │
     │                 │             fg / bg / other │
     ▼                 ▼                  │         │
 fork + exec      split, pipe(),          ▼         │
 (background)     fork N children    classify:      │
     │                 │             builtin /      │
     │                 │             external /     │
     │                 │             invalid        │
     └─────────────────┴──────────────────┴─────────┘
```
--- 

<img width="5993" height="8396" alt="diagram (1)" src="https://github.com/user-attachments/assets/e2f70387-4129-4395-aeef-b7adfb852bf6" />

---

## 📂 Project Structure

```
Minishell-in-C/
│
├── main.c
├── command.c
├── functions.c
├── jobs.c
├── pipe.c
├── shell.h
├── jobs.h
├── ext_commands.txt
├── Makefile
└── README.md
```

### File Description

| File | Description |
|------|-------------|
| `main.c` | Entry point; clears the screen and starts the shell loop |
| `command.c` | Input loop, built-in commands, external command execution |
| `functions.c` | Extracts the first word of a command and classifies it as built-in, external or invalid |
| `jobs.c` | Job table operations, background execution, signal handlers, `fg` / `bg` |
| `pipe.c` | Pipeline validation, command splitting, pipe creation and child wiring |
| `shell.h` | Shared declarations, ANSI color macros and standard includes |
| `jobs.h` | `Job` structure, enumerations and job-control prototypes |
| `ext_commands.txt` | List of external commands the shell is allowed to run |
| `Makefile` | Automates project compilation |
| `README.md` | Project documentation |

---

## 📋 Supported Commands

### Built-in Commands

| Command | Description |
|---------|-------------|
| `cd <dir>` | Change the current directory |
| `pwd` | Print the current working directory |
| `exit` | Exit the shell |
| `echo $$` | Print the shell's process ID |
| `echo $?` | Print the exit status of the last foreground command |
| `echo $SHELL` | Print the `SHELL` environment variable |
| `PS1=<text>` | Change the prompt text |

### Job Control Commands

| Command | Description |
|---------|-------------|
| `<command> &` | Run a command in the background |
| `jobs` | List all jobs |
| `jobs -l` | List all jobs with their PIDs |
| `jobs -r` | List running jobs only |
| `jobs -s` | List stopped jobs only |
| `fg %<id>` | Bring a job to the foreground (resumes it if stopped) |
| `bg` | Resume a stopped job in the background |
| `Ctrl+C` | Terminate the foreground job |
| `Ctrl+Z` | Stop the foreground job |

### Pipelines

| Syntax | Description |
|--------|-------------|
| `cmd1 \| cmd2` | Send the output of `cmd1` as the input of `cmd2` |
| `cmd1 \| cmd2 \| cmd3` | Chain any number of commands |

### External Commands

Any command listed in `ext_commands.txt` (for example `ls`, `cat`, `grep`, `ps`, `sleep`, `mkdir`, `gcc`) runs as a child process. Commands not found in the built-in list or in this file are reported as `command not found`.

---

## 💻 Usage

### Build and Run

```bash
git clone https://github.com/JITH1/Minishell-in-C.git
cd Minishell-in-C
make
./shell
```

> Run the shell from the project directory so it can locate `ext_commands.txt`.

### Typical Flow

1. Run a normal command such as `ls` or `pwd`
2. Chain commands with a pipe: `ls | grep c | wc -l`
3. Start a long task in the background: `sleep 60 &`
4. List jobs with `jobs` or `jobs -l`
5. Start a foreground command and press `Ctrl+Z` to stop it
6. Check stopped jobs with `jobs -s`
7. Resume it with `fg %<id>` or `bg`
8. Check the last exit status with `echo $?`
9. Change the prompt with `PS1=myshell>`
10. Type `exit` to quit

---

## 📋 Example

### Pipelines

```
minishell$~/home/user$ ls | grep .c
command.c
functions.c
jobs.c
main.c
pipe.c
minishell$~/home/user$ cat command.c | grep include | wc -l
2
```

### Background Job

```
minishell$~/home/user$ sleep 30 &
[1] 4821
minishell$~/home/user$ jobs
[1]+ Running	sleep 30
minishell$~/home/user$ jobs -l
[1]+ 4821 Running	sleep 30
```

### Stop and Resume

```
minishell$~/home/user$ sleep 100
^Z
[2]+    Stopped          sleep 100
minishell$~/home/user$ jobs -s
[2] Stopped	sleep 100
minishell$~/home/user$ fg %2
```

### Exit Status and Custom Prompt

```
minishell$~/home/user$ ls
minishell$~/home/user$ echo $?
0
minishell$~/home/user$ PS1=myshell>
myshell>~/home/user$
```

---

## 🔨 Compilation

### Using Makefile

Build the project using:

```bash
make
```

Clean the generated build files using:

```bash
make clean
```

### Manual Compilation

```bash
gcc -Wall -Wextra main.c command.c functions.c jobs.c pipe.c -o shell
```

---

## ▶️ Running the Program

After compilation, run:

```bash
./shell
```

---

## 🧠 Learning Outcomes

This project helped strengthen practical understanding of:

- Linux Process Management (`fork`, `exec`, `waitpid`)
- Inter-Process Communication using pipes
- File Descriptor Redirection with `dup2`
- Signal Handling (`SIGINT`, `SIGTSTP`, `SIGCONT`) and custom signal handlers for terminal-driven job control
- Job Control and Process States
- Linked Lists in C
- Structures and Pointers
- Dynamic Memory Allocation
- String Parsing and Tokenizing
- File Handling
- Exit Status Handling
- Modular Programming
- Makefile and Build Automation

---

## 🎯 Project Objective

The primary objective of this project is to understand how a real Unix shell works internally: how commands are parsed and launched as processes, how output is chained between processes through pipes, how the shell waits for or detaches from children, and how signals and process states are used to implement job control.

The project combines C programming, Linux system calls, inter-process communication, signal handling and linked lists to build a practical command-line shell.

---

## 🚧 Roadmap

- [ ] I/O redirection (`<`, `>`, `>>`)
- [ ] Wait for pipeline children and track them as jobs with proper exit status
- [ ] Background pipelines and built-ins inside pipelines
- [ ] Resolve commands through `PATH` instead of a static list
- [ ] Quoting and environment variable expansion
- [ ] `fg` / `bg` with multi-digit IDs and `%+` / `%-` specifiers
- [ ] Correct `Stopped` state display in `jobs`
- [ ] Send `SIGINT` instead of `SIGKILL` on `Ctrl+C`
- [ ] Replace the fixed prompt delay with proper output synchronization
- [ ] Command history and line editing

---

## 👨‍💻 About Me

**Jithin P**
Electronics and Communication Engineering Graduate | Embedded Systems & Firmware

I am an Electronics and Communication Engineering graduate with skills in Embedded Systems, Firmware Development, C/C++, Data Structures & Algorithms, and Linux Internals.

I focus on building strong foundations in programming and system-level development, with an emphasis on writing efficient and reliable software.

---

## 💻 Technical Skills

- C Programming
- C++
- Data Structures & Algorithms (DSA)
- Embedded Systems
- Firmware Development
- Linux Internals

---

## 🔗 Connect With Me

<p>
  <a href="https://www.linkedin.com/in/jithinjith"><img src="https://img.shields.io/badge/-LinkedIn-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white" alt="LinkedIn"></a>
  <a href="https://github.com/JITH1"><img src="https://img.shields.io/badge/-GitHub-181717?style=for-the-badge&logo=github&logoColor=white" alt="GitHub"></a>
</p>
