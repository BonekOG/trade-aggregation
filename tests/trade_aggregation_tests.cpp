#include "trade_aggregation/trade_aggregation.hpp"

#include <array>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace {

    using namespace trade_aggregation;
    using OhlcCandle = Candle<Open, High, Low, Close>;
    using AggregatorCandle = Candle<Open, Close, NumTrades<>>;
    using AlignedCandle = Candle<Open, Close, NumTrades<>, Volume>;

    constexpr array<Trade, 10> component_trades{{
        {1'684'677'200'000, 100.0, 10.0},
        {1'684'677'210'000, 101.0, -10.0},
        {1'684'677'220'000, 100.0, 20.0},
        {1'684'677'230'000, 102.0, 10.0},
        {1'684'677'240'000, 103.0, 10.0},
        {1'684'677'250'000, 104.0, -20.0},
        {1'684'677'260'000, 102.0, -10.0},
        {1'684'677'270'000, 101.0, 10.0},
        {1'684'677'280'000, 102.0, 30.0},
        {1'684'677'290'000, 105.0, 10.0},
    }};

    void check(bool condition, const char* expression, int line) {
        if (!condition) {
            cerr << "check failed at line " << line << ": " << expression << '\n';
            abort();
        }
    }

    #define CHECK(expression) check(static_cast<bool>(expression), #expression, __LINE__)

    vector<Trade> load_reference_trades(const string& filename) {
        const auto path = filesystem::path(TRADE_AGGREGATION_TEST_DATA_DIR) / filename;
        auto trades = load_trades_from_csv(path.string());

        CHECK(trades);

        return move(trades).value();
    }

    const vector<Trade>& bitmex_trades() {
        static const vector<Trade> trades = load_reference_trades("Bitmex_XBTUSD_1M.csv");

        return trades;
    }

    template <typename CandleType, typename Rule>
    vector<CandleType> aggregate_reference(span<const Trade> trades, Rule rule) {
        GenericAggregator<CandleType, Rule, Trade> aggregator(move(rule), false);

        return aggregate_all_trades<Trade, decltype(aggregator), CandleType>(trades, aggregator);
    }

    double round_to(double value, unsigned decimals) {
        const double factor = pow(10.0, decimals);

        return round(value * factor) / factor;
    }

    void test_generic_aggregator() {
        GenericAggregator<AggregatorCandle, TimeRule, Trade> aggregator(
            TimeRule(M1, TimestampResolution::Millisecond), false);
        size_t candle_counter = 0;

        for (const Trade& trade : bitmex_trades()) {
            if (aggregator.update(trade)) {
                ++candle_counter;
            }
        }

        CHECK(candle_counter == 5'953);
    }

    void test_candle_macro() {
        const AggregatorCandle candle;
        cout << "open: " << candle.component<Open>().value()
             << ", close: " << candle.component<Close>().value() << '\n';
    }

    void test_time_rule_differing_periods() {
        const auto m15_candles = aggregate_reference<OhlcCandle>(
            bitmex_trades(), TimeRule(M15, TimestampResolution::Millisecond));
        CHECK(m15_candles.size() == 396);

        const auto m5_candles = aggregate_reference<OhlcCandle>(
            bitmex_trades(), TimeRule(M5, TimestampResolution::Millisecond));
        CHECK(m5_candles.size() == 1'190);

        const auto h1_candles = aggregate_reference<OhlcCandle>(
            bitmex_trades(), TimeRule(H1, TimestampResolution::Millisecond));
        CHECK(h1_candles.size() == 99);
    }

    void test_time_rule_differing_timestamp_resolutions() {
        const auto& millisecond_trades = bitmex_trades();

        vector<Trade> microsecond_trades;
        vector<Trade> nanosecond_trades;

        microsecond_trades.reserve(millisecond_trades.size());
        nanosecond_trades.reserve(millisecond_trades.size());

        for (const Trade& trade : millisecond_trades) {
            microsecond_trades.push_back(Trade{trade.timestamp * 1'000, trade.price, trade.size});
            nanosecond_trades.push_back(Trade{trade.timestamp * 1'000'000, trade.price, trade.size});
        }

        const auto millisecond_candles = aggregate_reference<OhlcCandle>(
            millisecond_trades, TimeRule(M15, TimestampResolution::Millisecond));
        CHECK(millisecond_candles.size() == 396);

        const auto microsecond_candles = aggregate_reference<OhlcCandle>(
            microsecond_trades, TimeRule(M15, TimestampResolution::Microsecond));
        CHECK(microsecond_candles.size() == 396);

        const auto nanosecond_candles = aggregate_reference<OhlcCandle>(
            nanosecond_trades, TimeRule(M15, TimestampResolution::Nanosecond));
        CHECK(nanosecond_candles.size() == 396);
    }

    void test_aligned_time_rule() {
        const auto candles = aggregate_reference<AlignedCandle>(
            bitmex_trades(), AlignedTimeRule(M15, TimestampResolution::Millisecond));
        CHECK(candles.size() == 396);
        CHECK(candles[0].component<Open>().value() == 13'873.0);
        CHECK(candles[0].component<Close>().value() == 13'769.0);
        CHECK(candles[1].component<Open>().value() == 13'768.5);
        CHECK(candles[1].component<Close>().value() == 13'721.5);
    }

    void test_aligned_time_rule_volume() {
        const auto trades = load_reference_trades("Bitstamp_BTCEUR_1M.csv");
        const auto candles = aggregate_reference<AlignedCandle>(
            trades, AlignedTimeRule(M1, TimestampResolution::Microsecond));
        CHECK(candles[0].component<NumTrades<>>().value() == 10);
        CHECK(candles[0].component<Volume>().value() == 0.27458132);
    }

    void test_aligned_time_rule_trigger_on_0() {
        const array<Trade, 5> trades{{
            {1'712'656'800'000, 100.0, 10.0},
            {1'712'656'815'000, 101.0, -10.0},
            {1'712'656'860'000, 100.5, -10.0},
            {1'712'656'860'001, 102.0, -10.0},
            {1'712'656'935'000, 105.0, -10.0},
        }};
        const auto candles = aggregate_reference<OhlcCandle>(
            trades, AlignedTimeRule(M1, TimestampResolution::Millisecond));
        CHECK(candles.size() == 2);
        CHECK(candles[0].component<Open>().value() == 100.0);
        CHECK(candles[0].component<Close>().value() == 101.0);
        CHECK(candles[1].component<Open>().value() == 100.5);
        CHECK(candles[1].component<Close>().value() == 102.0);
    }

    void test_aligned_time_rule_candle_with_one_trade() {
        const array<Trade, 4> trades{{
            {1'712'656'800'000, 100.0, 10.0},
            {1'712'656'815'000, 101.0, -10.0},
            {1'712'656'861'000, 100.5, -10.0},
            {1'712'657'930'000, 102.0, -10.0},
        }};
        const auto candles = aggregate_reference<OhlcCandle>(
            trades, AlignedTimeRule(M1, TimestampResolution::Millisecond));
        CHECK(candles.size() == 2);
        CHECK(candles[0].component<Open>().value() == 100.0);
        CHECK(candles[0].component<Close>().value() == 101.0);
        CHECK(candles[1].component<Open>().value() == 100.5);
        CHECK(candles[1].component<Close>().value() == 100.5);
    }

    void test_tick_rule() {
        const auto candles = aggregate_reference<OhlcCandle>(bitmex_trades(), TickRule(1'000));
        CHECK(candles.size() == 1'000);
    }

    void test_relative_price_rule() {
        auto result = RelativePriceRule::create(0.01);
        CHECK(result);
        auto rule = move(result).value();

        CHECK(!rule.should_trigger(Trade{0, 100.0, 10.0}, OhlcCandle{}));
        CHECK(!rule.should_trigger(Trade{0, 100.5, 10.0}, OhlcCandle{}));
        CHECK(rule.should_trigger(Trade{0, 101.0, 10.0}, OhlcCandle{}));
        CHECK(!rule.should_trigger(Trade{0, 100.5, 10.0}, OhlcCandle{}));
        CHECK(rule.should_trigger(Trade{0, 99.0, 10.0}, OhlcCandle{}));
    }

    void test_relative_price_rule_real_data() {
        auto rule = RelativePriceRule::create(0.01);
        CHECK(rule);
        const auto candles = aggregate_reference<OhlcCandle>(bitmex_trades(), move(rule).value());
        CHECK(!candles.empty());

        constexpr double threshold = 0.005;
        for (const OhlcCandle& candle : candles) {
            const double high = candle.component<High>().value();
            const double low = candle.component<Low>().value();
            const double close = candle.component<Close>().value();
            const double open = candle.component<Open>().value();
            CHECK((high - low) / low >= threshold);
            CHECK(abs(close - open) / open >= threshold);
        }
    }

    void test_average_price() {
        AveragePrice component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
        }
        
        CHECK(component.value() == 102.0);
    }

    void test_close() {
        Close component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
            CHECK(component.value() == trade.price);
        }

        CHECK(component.value() == component_trades.back().price);
    }

    void test_close_timestamp() {
        CloseTimeStamp component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
        }

        CHECK(component.value() == 1'684'677'290'000);
    }

    void test_trade_direction_ratio() {
        DirectionalTradeRatio component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
        }

        CHECK(component.value() == 0.7);
    }

    void test_volume_direction_ratio() {
        DirectionalVolumeRatio component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
        }

        CHECK(round_to(component.value(), 4) == 0.7143);
    }

    void test_high() {
        High component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
        }

        CHECK(component.value() == 105.0);
    }

    void test_low() {
        Low component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
        }

        CHECK(component.value() == 100.0);
    }

    void test_median_price() {
        MedianPrice component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
        }

        CHECK(component.value() == 102.0);
    }

    void test_num_trades() {
        NumTrades<> component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
        }

        CHECK(component.value() == 10);
    }

    void test_open() {
        Open component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
            CHECK(component.value() == component_trades.front().price);
        }
    }

    void test_open_timestamp() {
        OpenTimeStamp component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
        }

        CHECK(component.value() == 1'684'677'200'000);
    }

    void test_time_velocity() {
        TimeVelocity component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
        }

        CHECK(round_to(component.value(), 3) == 0.011);
    }

    void test_trades() {
        Trades<Trade> component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
        }

        CHECK(component.value() == vector<Trade>(component_trades.begin(), component_trades.end()));
    }

    void test_volume() {
        Volume component;
        double sum = 0.0;

        for (const Trade& trade : component_trades) {
            sum += abs(trade.size);
            component.update(trade);
            CHECK(component.value() == sum);
        }
    }

    void test_weighted_price() {
        WeightedPrice component;

        for (const Trade& trade : component_trades) {
            component.update(trade);
        }

        CHECK(component.value() == 102.0);
    }

    void test_millisecond_period_from_non_zero_secs_panic() {
        bool rejected_zero = false;

        try {
            static_cast<void>(MillisecondPeriod::from_non_zero_secs(0));
        } catch (const invalid_argument&) {
            rejected_zero = true;
        }

        CHECK(rejected_zero);
    }

    void test_millisecond_period_from_non_zero_secs() {
        CHECK(MillisecondPeriod::from_non_zero_secs(1).get() == 1'000);
        CHECK(MillisecondPeriod::from_non_zero_secs(60).get() == 60'000);
    }

    void test_candle_volume_from_time_period() {
        CHECK(round_to(candle_volume_from_time_period(100.0, 10.0, 5.0), 3) == 0.035);
        CHECK(round_to(candle_volume_from_time_period(100.0, 10.0, 10.0), 3) == 0.069);
        CHECK(round_to(candle_volume_from_time_period(200.0, 10.0, 10.0), 3) == 0.139);
        CHECK(round_to(candle_volume_from_time_period(50.0, 10.0, 10.0), 3) == 0.035);
        CHECK(round_to(candle_volume_from_time_period(100.0, 5.0, 5.0), 3) == 0.069);
        CHECK(round_to(candle_volume_from_time_period(100.0, 5.0, 10.0), 3) == 0.139);
    }

    void test_welford_online() {
        WelfordOnline component;

        for (const double value : {1.0, 2.0, 1.0, 2.0}) {
            component.add(value);
        }

        CHECK(round_to(component.std_dev(), 4) == 0.5774);
    }

}

int main() {
    test_generic_aggregator();
    test_candle_macro();
    test_time_rule_differing_periods();
    test_time_rule_differing_timestamp_resolutions();
    test_aligned_time_rule();
    test_aligned_time_rule_volume();
    test_aligned_time_rule_trigger_on_0();
    test_aligned_time_rule_candle_with_one_trade();
    test_tick_rule();
    test_relative_price_rule();
    test_relative_price_rule_real_data();
    test_average_price();
    test_close();
    test_close_timestamp();
    test_trade_direction_ratio();
    test_volume_direction_ratio();
    test_high();
    test_low();
    test_median_price();
    test_num_trades();
    test_open();
    test_open_timestamp();
    test_time_velocity();
    test_trades();
    test_volume();
    test_weighted_price();
    test_millisecond_period_from_non_zero_secs_panic();
    test_millisecond_period_from_non_zero_secs();
    test_candle_volume_from_time_period();
    test_welford_online();
}
