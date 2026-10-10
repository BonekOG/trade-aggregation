#include "trade_aggregation/trade_aggregation.hpp"

#include <iostream>
#include <vector>

using namespace std;

int main() {
    using namespace trade_aggregation;
    using OhlcCandle = Candle<Open, High, Low, Close, Volume, NumTrades<>>;

    GenericAggregator<OhlcCandle, TimeRule, Trade> aggregator(TimeRule(M1, TimestampResolution::Millisecond), false);

    const vector<Trade> trades{
        {1'000, 100.0, 2.0},
        {31'000, 102.0, -1.0},
        {61'001, 101.0, 3.0},
        {121'001, 103.0, -2.0},
    };

    for (const Trade& trade : trades) {
        if (auto candle = aggregator.update(trade)) {
            cout << "candle created: open=" << candle->component<Open>().value()
                      << ", high=" << candle->component<High>().value()
                      << ", low=" << candle->component<Low>().value()
                      << ", close=" << candle->component<Close>().value()
                      << ", volume=" << candle->component<Volume>().value()
                      << ", trades=" << candle->component<NumTrades<>>().value() << '\n';
        }
    }
}
