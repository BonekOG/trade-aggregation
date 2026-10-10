#pragma once

#include "trade_aggregation/aggregation_rule.hpp"

#include <concepts>
#include <optional>
#include <utility>

namespace trade_aggregation {

    template <typename AggregatorType, typename Candle, typename TradeType>
    concept AggregatorFor = ModularCandleFor<Candle, TradeType> && requires(
                                                                       AggregatorType& aggregator, const TradeType& trade) {
        { aggregator.update(trade) } -> std::same_as<std::optional<Candle>>;
        { aggregator.unfinished_candle() } -> std::same_as<const Candle&>;
    };

    template <typename Candle, typename Rule, typename TradeType>
        requires ModularCandleFor<Candle, TradeType> && AggregationRuleFor<Rule, Candle, TradeType>
    class GenericAggregator {
        public:
            explicit GenericAggregator(Rule aggregation_rule, bool include_trade_that_triggered_rule)
                : aggregation_rule_(std::move(aggregation_rule)),
                  include_trade_that_triggered_rule_(include_trade_that_triggered_rule) {}

            [[nodiscard]] std::optional<Candle> update(const TradeType& trade) {
                if (!aggregation_rule_.should_trigger(trade, candle_)) {
                    candle_.update(trade);
                    return std::nullopt;
                }

                if (include_trade_that_triggered_rule_) {
                    candle_.update(trade);
                }

                Candle completed = candle_;
                candle_.reset();
                candle_.update(trade);
                
                return completed;
            }

            [[nodiscard]] const Candle& unfinished_candle() const noexcept { return candle_; }
            [[nodiscard]] const Rule& aggregation_rule() const noexcept { return aggregation_rule_; }
            [[nodiscard]] bool includes_triggering_trade() const noexcept {
                return include_trade_that_triggered_rule_;
            }

        private:
            Candle candle_{};
            Rule aggregation_rule_;
            bool include_trade_that_triggered_rule_;
    };
}
