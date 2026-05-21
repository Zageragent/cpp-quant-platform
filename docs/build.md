# Build

Default build requires CMake 3.25+ and a C++20 compiler. No third-party dependency is required for the default targets. Presets: debug, release, asan, ubsan, ci.

## Dev container / WSL

The repository has a `.devcontainer/` setup for VS Code Dev Containers. Use it when you want a clean Linux C++ toolchain without manually installing packages in WSL.

Prerequisites on Windows:

- WSL 2.
- VS Code.
- VS Code extensions: **WSL** and **Dev Containers**.
- Docker Desktop with WSL integration enabled, or another Docker Engine reachable from WSL.

From a WSL shell:

```bash
cd ~/cpp-quant-platform
code .
```

Then choose **Dev Containers: Reopen in Container**. The container uses Ubuntu 24.04 and installs `build-essential`, `cmake`, `ninja-build`, `gdb`, `valgrind`, and related C++ tooling. The first container creation runs the debug configure/build/test cycle automatically.

Inside the container:

```bash
cmake --preset debug
cmake --build --preset debug --parallel
ctest --preset debug --output-on-failure
```
