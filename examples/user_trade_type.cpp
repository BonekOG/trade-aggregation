#include "trade_aggregation/trade_aggregation.hpp"

#include <iostream>
#include <vector>

using namespace std;

struct ExchangeTrade {
    int64_t exchange_time_us{};
    double execution_price{};
    double quantity{};

    [[nodiscard]] int64_t timestamp() const noexcept { 
        return exchange_time_us;
    }

    [[nodiscard]] double price() const noexcept {
        return execution_price;
    }

    [[nodiscard]] double size() const noexcept {
        return quantity;
    }

    [[nodiscard]] trade_aggregation::TimestampResolution timestamp_resolution() const noexcept {
        return trade_aggregation::TimestampResolution::Microsecond;
    }
};

int main() {
    using namespace trade_aggregation;
    using CandleType = Candle<Open, Close, Volume, NumTrades<>>;

    GenericAggregator<CandleType, TickRule, ExchangeTrade> aggregator(TickRule(2), false);

    const vector<ExchangeTrade> trades{
        {1'000'000, 100.25, 2.0},
        {1'100'000, 100.50, -1.0},
        {1'200'000, 100.75, 3.0},
    };

    for (const auto& candle : aggregate_all_trades<ExchangeTrade, decltype(aggregator), CandleType>(trades, aggregator)) {
        cout << "open=" << candle.component<Open>().value()
             << ", close=" << candle.component<Close>().value()
             << ", volume=" << candle.component<Volume>().value()
             << ", trades=" << candle.component<NumTrades<>>().value() << '\n';
    }
}
