# Trade Aggregation for C++

A modular C++ library that aggregates trades into candles, with support for
incremental updates. Aggregation rules, candle components, and input trade types
can be customized.

This project is inspired by the repo of Mathis Wellmann. [MathisWellmann/trade_aggregation-rs](https://github.com/MathisWellmann/trade_aggregation-rs),


The aggregation examples from the original Rust project have been adapted to C++:

- [Streaming OHLC aggregation](examples/streaming_aggregate_ohlc.cpp)
- [Batch OHLC aggregation](examples/aggregate_all_ohlc.cpp)
