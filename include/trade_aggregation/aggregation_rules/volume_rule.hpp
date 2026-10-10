#pragma once

#include "trade_aggregation/aggregation_rule.hpp"
#include "trade_aggregation/errors.hpp"
#include "trade_aggregation/types.hpp"

#include <cmath>

namespace trade_aggregation {

    class VolumeRule {
        public:
            [[nodiscard]] static Result<VolumeRule> create(double threshold_volume, By by) {
                if (threshold_volume <= 0.0) {
                    return Error(ErrorCode::InvalidParam, "volume threshold must be greater than zero");
                }

                return VolumeRule(threshold_volume, by);
            }

            template <typename TradeType, typename Candle>
            [[nodiscard]] bool should_trigger(const TradeType& trade, const Candle&) {
                if (cumulative_volume_ >= threshold_volume_) {
                    cumulative_volume_ -= threshold_volume_;
                }

                cumulative_volume_ += by_ == By::Quote ? std::abs(trade_size(trade)) : std::abs(trade_size(trade)) / trade_price(trade);

                return cumulative_volume_ >= threshold_volume_;
            }

            [[nodiscard]] double cumulative_volume() const noexcept { return cumulative_volume_; }

        private:
            VolumeRule(double threshold_volume, By by) : by_(by), cumulative_volume_(0.0), threshold_volume_(threshold_volume) {}

            By by_;
            double cumulative_volume_;
            double threshold_volume_;
    };
}
