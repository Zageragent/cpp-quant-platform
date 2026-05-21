# Deterministic Replay

Replay events sort by event_time then sequence_number. A ReplayClock never moves backwards. Sinks receive events in deterministic order and errors abort the run. Tests cover equal-timestamp tie-breaking and backward-time rejection.

## Ordering enforcement

Replay now requires a strict `(event_time, sequence_number)` ordering during engine execution. Duplicate `(time, sequence)` pairs are rejected rather than relying on implementation-defined sort behavior. `max_events` is checked before pulling the next event so bounded replay does not consume an extra event.
