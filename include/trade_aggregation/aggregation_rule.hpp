#pragma once

#include "trade_aggregation/modular_candle.hpp"

#include <concepts>

namespace trade_aggregation {

    template <typename Rule, typename Candle, typename TradeType>
    concept AggregationRuleFor = ModularCandleFor<Candle, TradeType> && requires(
                                                                            Rule& rule, const TradeType& trade, const Candle& candle) {
        { rule.should_trigger(trade, candle) } -> std::convertible_to<bool>;
    };
}
