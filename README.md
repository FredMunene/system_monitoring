# 🖥️ System Monitor

A desktop system monitor built in **C++** using **Dear ImGui** for the user interface. This application displays live information on CPU, memory, processes, thermal state, fan performance, and network activity by leveraging the Linux `/proc` filesystem.

---

## 📌 Project Objectives

This project demonstrates:

- Solid understanding of **programming logic** in C++.
- Ability to work with **new languages** and integrate with external libraries.
- Use of **Dear ImGui** to build a responsive GUI.
- Working knowledge of **Linux system monitoring** via `/proc`.

---

## 🧱 Project Structure
```
$ tree system-monitor
├── header.h
├── imgui                                 // <-- ImGui APIs
│   └── lib
│       ├── backend                       // <-- ImGui backend
│       │   ├── imgui_impl_opengl3.cpp
│       │   ├── imgui_impl_opengl3.h
│       │   ├── imgui_impl_sdl.cpp
│       │   └── imgui_impl_sdl.h
│       ├── gl3w
│       │   └── GL
│       │       ├── gl3w.c
│       │       ├── gl3w.h
│       │       └── glcorearb.h
│       ├── imconfig.h
│       ├── imgui.cpp
│       ├── imgui_demo.cpp
│       ├── imgui_draw.cpp
│       ├── imgui.h
│       ├── imgui_internal.h
│       ├── imgui_tables.cpp
│       ├── imgui_widgets.cpp
│       ├── imstb_rectpack.h
│       ├── imstb_textedit.h
│       └── imstb_truetype.h
├── main.cpp                             // <-- main file, where the application will
├── Makefile                             //     render (main loop)
├── mem.cpp                              // <-- memory resources and processes information
├── network.cpp                          // <-- network resources
└── system.cpp                           // <-- all system resources

```


---

## ⚙️ Features

### 🧠 System Overview

- OS type, hostname, and current logged-in user.
- Task overview: running, sleeping, zombie, etc.
- CPU model and core usage.

### 🧮 CPU / Fan / Thermal Tabs

- Real-time graphs for CPU usage, fan speed, and thermal sensors.
- Adjustable graph FPS and Y-scale.
- Pause/resume animation.
- CPU usage overlay (%).
- Fan: active status, speed, and level.
- Thermal: current CPU temperature overlay.

### 🧵 Memory & Processes

- Live visualizations of:
  - RAM usage
  - SWAP usage
  - Disk usage
- Process table with:
  - PID
  - Name
  - State
  - CPU %
  - Memory %
- Multi-row selection and filter textbox.

### 🌐 Network

- Displays IPv4 interfaces (e.g., `lo`, `wlp5s0`).
- Two data tables:
  - RX (Receive): bytes, packets, errs, drop, etc.
  - TX (Transmit): bytes, packets, errs, colls, etc.
- Dynamic visual usage bars:
  - Scales data values (e.g., 431.78 MB instead of 0.42 GB or 442144 KB)
  - Ranges from 0 GB to 2 GB

---

## 🛠️ Getting Started

### ✅ Prerequisites

- Linux-based OS
- C++ compiler (e.g., `g++`)
- SDL2:
  ```bash
  sudo apt install libsdl2-dev

### 🔧 Build Instructions

```make
./system-monitor
```
### 📚 Learning Outcomes

By working on this project, you'll gain experience in:
- C++ development
- GUI design using Dear ImGui
- System monitoring with /proc
- Performance visualization and UI responsiveness

### ❗ Issues & Contributions

Found a bug? Want to contribute or suggest improvements?
Submit an issue or create a pull request.
### 👨‍💻 Author
[Fred Gitonga](https://github.com/fredmunene)


## Desktop System Monitor
- app to monitor computer system recources and perfomance, such as CPU,RAM, SWAP,Fan, Network and more. 
Use [Dear ImGui](https://github.com/ocornut/imgui/wiki#about-the-imgui-paradigm), a library for the GUI.

## Dear ImGui
 ImGui is an immediate mode graphic user interface, as the name implies it: "ImGui". 
 + Immediate mode: the application state is separated from the graphics library. It is the application responsibility for drawing commands when necessary. In other words, in immediate mode the client calls directly results in the rendering of graphics objects to the display.

The files are added to the existing project. To integrate Dear ImGui you must use a backend, this backend passes mouse/keyboard/gamepad inputs and a variety of settings. It is in charge of rendering the resulting vertices. 

```
apt install libsdl2-dev
```

