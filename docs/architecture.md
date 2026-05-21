# Architecture

Architecture is layered: core -> data/schema -> replay/features/labels -> signals/execution/portfolio -> apps. Dependencies should only point downward. Replay uses timestamp plus sequence ordering for deterministic iteration. Features consume market events; labels consume future windows only through explicit leakage guards.
