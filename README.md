# Qt Task Manager

A lightweight Linux task manager application written in **C++** using the **Qt framework**.  
The project focuses on low-level process monitoring by reading data directly from the `/proc` filesystem.

## Features

- List of running processes
- CPU usage per process
- Memory usage per process
- Real-time system updates
- Send signals to processes (kill, stop, continue)
- Custom Qt model/view architecture
- CPU usage visualization using custom delegates

## Architecture Overview

- **ProcessManager**  
  Reads and parses system and process information from `/proc`

- **ProcessModel (QAbstractTableModel)**  
  Provides process data to Qt views

- **Proxy Model**  
  Used for sorting and filtering processes

- **Custom Delegates**  
  Used for rendering CPU usage bars inside table cells

## Build Instructions

### Requirements

- Linux
- C++17 compatible compiler
- Qt 6 (Qt 5 may also work)
- CMake

### Build

```bash
git clone https://github.com/Keralix/ProcessManager
cd ProcessManager

mkdir build
cd build
cmake ..
make
