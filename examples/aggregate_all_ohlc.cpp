#include "trade_aggregation/trade_aggregation.hpp"

#include <cstdlib>
#include <iostream>
#include <utility>

using namespace std;
using namespace trade_aggregation;

using OhlcCandle = Candle<Open, High, Low, Close>;

int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "usage: aggregate_all_ohlc <trades.csv>\n";
        return EXIT_FAILURE;
    }

    auto loaded_trades = load_trades_from_csv(argv[1]);

    if (!loaded_trades) {
        cerr << loaded_trades.error().message() << '\n';
        return EXIT_FAILURE;
    }

    vector<Trade> trades = move(loaded_trades).value();
    GenericAggregator<OhlcCandle, TimeRule, Trade> aggregator(TimeRule(M1, TimestampResolution::Millisecond), false);

    const auto candles = aggregate_all_trades<Trade, decltype(aggregator), OhlcCandle>(trades, aggregator);

    cout << "aggregated " << trades.size() << " trades into " << candles.size() << " candles\n";

    for (const auto& candle : candles) {
        cout << candle.component<Open>().value() << ','
             << candle.component<High>().value() << ','
             << candle.component<Low>().value() << ','
             << candle.component<Close>().value() << '\n';
    }

    return EXIT_SUCCESS;
}

