# Deterministic Replay

Replay events sort by event_time then sequence_number. A ReplayClock never moves backwards. Sinks receive events in deterministic order and errors abort the run. Tests cover equal-timestamp tie-breaking and backward-time rejection.
