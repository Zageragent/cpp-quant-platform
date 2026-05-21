# C++ Quant Platform

Research-grade C++20 quantitative research platform foundation. This repository is intentionally **paper/research-first**: it provides deterministic replay, typed market-data schemas, rolling features, leakage-safe labels, baseline signals, and portfolio accounting primitives. It is **not** live-trading software and does not provide investment advice.

## Current implementation status

Implemented in the initial vertical slice:

- Dependency-free CMake/CTest build that works on bare Linux.
- Strong-ish domain types for symbols, prices, quantities, notionals, timestamps, and errors/results.
- OHLCV bars, trades, quotes, validation rules, and CSV OHLCV reader.
- Deterministic replay event source/clock/engine with timestamp + sequence ordering.
- Rolling windows, simple/log returns, rolling mean, rolling variance.
- Leakage guard, forward-return labels, and triple-barrier labels.
- Momentum signal baseline and simple portfolio accounting.
- CLI smoke command, unit tests, and dependency-free smoke benchmark.

Planned / feature-gated later:

- Arrow/Parquet storage.
- GoogleTest/Google Benchmark integration.
- OSQP portfolio optimizer.
- Paper broker adapters and live execution safety stack.

## Quick start

```bash
cmake --preset debug
cmake --build --preset debug --parallel
ctest --preset debug --output-on-failure

cmake --preset release
cmake --build --preset release --parallel
./build/release/qp smoke
./build/release/qp_bench_smoke --iterations 100000
```

## Windows / WSL dev container

This repo includes a VS Code dev container for Windows machines using WSL 2 and Docker Desktop.

From an elevated Windows PowerShell prompt, run the prerequisite installer/checker:

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force
.\scripts\windows\setup-devcontainer.ps1
```

Then open the repository from WSL:

```bash
git clone https://github.com/Zageragent/cpp-quant-platform.git
cd cpp-quant-platform
code .
```

In VS Code, run **Dev Containers: Reopen in Container**.

The container installs the Linux C++ toolchain, CMake 3.25+, Ninja, GDB, Valgrind, GitHub CLI, and VS Code C++/CMake extensions. On first create, it runs:

```bash
cmake --preset debug
cmake --build --preset debug --parallel
ctest --preset debug --output-on-failure
```

See [docs/windows-devcontainer.md](docs/windows-devcontainer.md) for Docker Desktop, WSL integration, and troubleshooting details.

## Design principles

- C++20 or newer.
- Linux-first, cross-platform where practical.
- Deterministic research before advanced prediction.
- Same event/signal path should work across replay, backtest, and future paper trading.
- Explicit schema/versioning and UTC timestamp semantics.
- No lookahead bias; labels and features are separated with leakage guards.
- Heavy dependencies are optional, not required for the default build.
