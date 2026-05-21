# Dataset Artifacts v1

Implemented dependency-free CSV schemas:

- `qp.dataset.bars.v1`: `schema_version,symbol,start_ns,end_ns,open,high,low,close,volume`
- `qp.dataset.features.v1`: `schema_version,ts_ns,symbol,name,value`
- `qp.dataset.labels.v1`: `schema_version,sample_ts_ns,label_start_ns,label_end_ns,symbol,name,kind,value,leakage_safe`
- `qp.dataset.signals.v1`: `schema_version,ts_ns,symbol,name,side,strength`

Writers validate rows before writing and use UTC Unix nanoseconds.
