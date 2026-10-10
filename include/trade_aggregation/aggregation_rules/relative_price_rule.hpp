#pragma once

#include "trade_aggregation/aggregation_rule.hpp"
#include "trade_aggregation/errors.hpp"

#include <cmath>

namespace trade_aggregation {

    class RelativePriceRule {
        public:
            [[nodiscard]] static Result<RelativePriceRule> create(double threshold_fraction) {
                if (threshold_fraction <= 0.0) {
                    return Error(ErrorCode::InvalidParam, "relative price threshold must be greater than zero");
                }

                return RelativePriceRule(threshold_fraction);
            }

            template <typename TradeType, typename Candle>
            [[nodiscard]] bool should_trigger(const TradeType& trade, const Candle&) {
                if (init_) {
                    init_ = false;
                    init_price_ = trade_price(trade);
                    return init_;
                }

                const double price_delta = std::abs(trade_price(trade) - init_price_) / init_price_;

                if (price_delta >= threshold_fraction_) {
                    init_price_ = trade_price(trade);
                    return true;
                }
                
                return false;
            }

        private:
            explicit RelativePriceRule(double threshold_fraction)
                : init_(true), init_price_(0.0), threshold_fraction_(threshold_fraction) {}

            bool init_;
            double init_price_;
            double threshold_fraction_;
    };
}
