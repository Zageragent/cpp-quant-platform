# ADR 0001: Core principles

Decision: use C++20, CMake, UTC nanosecond timestamps, deterministic replay, explicit schema validation, and feature-gated heavy dependencies.

Status: accepted.

Consequences: the default build stays small and testable; scalable storage and optimizers are added behind options later.
