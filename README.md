# 🚀 Multi-Container Runtime System (OS Jackfruit)

A lightweight container runtime built as part of the **OS Jackfruit problem**, demonstrating core Operating System concepts using low-level Linux primitives.

This project implements a simple container engine using `chroot`, process management, inter-process communication (IPC), logging, and kernel-level monitoring.

---

## 👥 Team Information

- **Nagalapuram Sai Mukesh Reddy** — SRN: PES1UG24AM170
- **Mallela Rishyendra** — SRN: PES1UG24AM154

---

## ⚙️ Features

- Multi-container execution (`alpha`, `beta`)
- Supervisor-based container lifecycle management
- CLI commands: `start`, `stop`, `ps`, `logs`
- IPC using UNIX domain sockets
- Logging using pipes
- CPU vs I/O workload scheduling demonstration
- Kernel module monitoring (soft & hard limits)
- Clean teardown with no zombie processes

---

## 🛠️ Setup & Execution

### 1. Build Project
```bash
make
```

### 2. Load Kernel Module
```bash
sudo insmod monitor.ko
```

### 3. Start Supervisor
```bash
sudo ./engine supervisor
```

### 4. Prepare Root Filesystems
```bash
cp -a rootfs-base rootfs-alpha
cp -a rootfs-base rootfs-beta
```

### 5. Run Containers

#### Multi-container execution
```bash
sudo ./engine start alpha ../rootfs-alpha /cpu_hog 20
sudo ./engine start beta  ../rootfs-beta  /cpu_hog 20
```

#### Scheduling experiment (CPU vs IO)
```bash
sudo ./engine start alpha ../rootfs-alpha /cpu_hog 20
sudo ./engine start beta  ../rootfs-beta  /io_pulse
```

### 6. Check Container Status
```bash
sudo ./engine ps
```

### 7. View Logs
```bash
sudo ./engine logs alpha
sudo ./engine logs beta
```

### 8. Stop Containers
```bash
sudo ./engine stop alpha
sudo ./engine stop beta
```

### 9. Kernel Monitoring
```bash
sudo ./engine start alpha ../rootfs-alpha /memory_hog
sudo dmesg | grep monitor
```

### 10. Cleanup
```bash
sudo pkill -9 engine
sudo rm -f /tmp/engine_socket
```

---

## 📸 Demo Screenshots

| # | Feature |
|--|--------|
| 1 | Multi-container supervision |
| 2 | Metadata tracking (`ps`) |
| 3 | Logging output |
| 4 | CLI and IPC |
| 5 | Soft limit warning |
| 6 | Hard limit enforcement |
| 7 | Scheduling experiment |
| 8 | Clean teardown |

---

## 🧠 System Design

### 🧩 Container Isolation
- Implemented using `chroot`
- Each container runs as a separate process
- No full namespace isolation

---

### 🧭 Supervisor
- Central controller for all containers
- Handles `start`, `stop`, `ps`, and `logs`
- Tracks container state
- Ensures no zombie processes remain

---

### 🔌 IPC (CLI ↔ Supervisor)
- Implemented using UNIX domain sockets
- CLI sends commands to supervisor
- Supervisor executes and returns results

---

### 📜 Logging
- Pipes capture container output
- Logs accessed via `engine logs`
- Separate logs maintained per container

---

### 🧬 Kernel Monitoring (monitor.ko)
- Registers container processes
- Simulates memory monitoring
- Generates:

```
monitor: Registered container alpha
monitor: Soft limit exceeded for alpha
monitor: Killing container alpha
```

---

### ⚖️ Scheduling Experiment

Two workloads are used:

#### CPU-bound (`cpu_hog`)
- Continuous execution
- High CPU usage

#### I/O-bound (`io_pulse`)
- Periodic execution
- Yields CPU frequently

**Observation:**
- CPU-bound process runs continuously  
- I/O-bound process runs intermittently  
- Demonstrates fair scheduling behavior  

---

## ⚖️ Design Decisions & Tradeoffs

| Choice | Reason | Limitation |
|------|--------|-----------|
| `chroot` | Simple isolation | Not fully secure |
| Single supervisor | Easy control | Single point of failure |
| Pipes for logging | Simple implementation | Limited scalability |
| Kernel module simulation | Easy demonstration | Not real memory tracking |

---

## 📊 Key Observations

- Multiple containers run concurrently  
- CLI communicates with supervisor via IPC  
- Logs correctly capture container output  
- Kernel module generates expected monitoring output  
- Scheduler fairly distributes CPU between workloads  

---

## 🧾 Conclusion

This project demonstrates key OS concepts:

- Process management  
- Containerization using `chroot`  
- Inter-process communication  
- Logging systems  
- Kernel interaction  
- Scheduling behavior  

A minimal yet functional container runtime built using low-level Linux primitives.
