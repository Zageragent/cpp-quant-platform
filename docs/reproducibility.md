# Reproducibility

Runs must capture config, input data identifiers, git SHA, compiler/build mode, RNG seed, schema versions, and output artifact paths. Avoid unordered reductions and implicit timezones. Default timestamp unit is UTC nanoseconds since Unix epoch.

## Implemented artifact fingerprints

The run manifest now includes best-effort dependency-free FNV-1a fingerprints for the effective config and input data file, along with schema versions, compiler text, qp version, and artifact paths. These fingerprints are identifiers for reproducibility checks, not cryptographic security hashes.
