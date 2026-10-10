#pragma once

#include "trade_aggregation/aggregator.hpp"
#include "trade_aggregation/errors.hpp"
#include "trade_aggregation/types.hpp"

#include <span>
#include <string_view>
#include <utility>
#include <vector>

namespace trade_aggregation {

    [[nodiscard]] double candle_volume_from_time_period(
        double total_volume,
        double total_time_days,
        double target_time_minutes) noexcept;

    template <typename TradeType, typename AggregatorType, typename CandleType>
        requires AggregatorFor<AggregatorType, CandleType, TradeType>
    [[nodiscard]] std::vector<CandleType> aggregate_all_trades(std::span<const TradeType> trades, AggregatorType& aggregator) {
        std::vector<CandleType> candles;

        for (const TradeType& trade : trades) {
            if (auto candle = aggregator.update(trade)) {
                candles.push_back(std::move(*candle));
            }
        }
        
        return candles;
    }

    [[nodiscard]] Result<std::vector<Trade>> load_trades_from_csv(std::string_view filename);
}
