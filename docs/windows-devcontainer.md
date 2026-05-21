# Windows + WSL Dev Container Setup

This project is designed to build on Windows through the VS Code Dev Containers workflow:

Windows host → WSL 2 Ubuntu workspace → Docker Desktop Linux container → CMake build/test

That keeps the project on a real Linux C++ toolchain while still using a Windows machine.

## One-command prerequisite setup

From an elevated Windows PowerShell prompt, run this from the repository root:

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force
.\scripts\windows\setup-devcontainer.ps1
```

The script checks or installs:

- WSL 2
- Git for Windows
- Docker Desktop
- Visual Studio Code
- VS Code extensions:
  - WSL
  - Dev Containers
  - C/C++
  - CMake Tools
  - Docker

If you only want to check without installing anything:

```powershell
.\scripts\windows\setup-devcontainer.ps1 -NoInstall
```

## Docker Desktop requirements

After Docker Desktop is installed:

1. Open Docker Desktop.
2. Finish first-run onboarding if prompted.
3. Go to **Settings → Resources → WSL integration**.
4. Enable integration for your Ubuntu WSL distro.
5. Apply/restart Docker Desktop if prompted.

Verify from PowerShell:

```powershell
docker version
```

Verify from WSL:

```bash
docker version
```

Both should show a Docker client and server.

## Clone and open from WSL

From an Ubuntu/WSL shell:

```bash
git clone https://github.com/Zageragent/cpp-quant-platform.git
cd cpp-quant-platform
code .
```

In VS Code, select:

```text
Dev Containers: Reopen in Container
```

The first container creation automatically runs:

```bash
cmake --preset debug
cmake --build --preset debug --parallel
ctest --preset debug --output-on-failure
```

## Manual build inside the container

```bash
cmake --preset debug
cmake --build --preset debug --parallel
ctest --preset debug --output-on-failure

cmake --preset release
cmake --build --preset release --parallel
./build/release/qp smoke
./build/release/qp_bench_smoke --iterations 100000
```

## Troubleshooting

### `docker: command not found` in WSL

Docker Desktop WSL integration is not enabled for your distro. Enable it in Docker Desktop under **Settings → Resources → WSL integration**.

### Docker server is unreachable

Open Docker Desktop and wait until it says the engine is running. Then retry:

```bash
docker version
```

### VS Code opens Windows folder instead of WSL folder

Open the repo from a WSL shell with `code .`, not from Windows Explorer. The bottom-left VS Code status bar should show `WSL: Ubuntu` before reopening in the container.

### Dev container rebuild needed

In VS Code, run:

```text
Dev Containers: Rebuild Container
```
