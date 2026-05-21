# Data Model

Initial schemas: Instrument, OHLCV Bar, Trade, Quote, FeatureValue, LabelValue, Signal, Order/Fill future, Portfolio. Bars use [start,end) UTC nanosecond intervals. OHLCV validation rejects inconsistent high/low, non-positive prices, and negative volume.
