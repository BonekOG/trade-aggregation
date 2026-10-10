#pragma once

#include "trade_aggregation/aggregation_rule.hpp"
#include "trade_aggregation/types.hpp"

#include <cstdint>
#include <limits>
#include <stdexcept>

namespace trade_aggregation {

    class TimeRule {
        public:
            TimeRule(MillisecondPeriod period_ms, TimestampResolution resolution)
                : reference_timestamp_(0), period_in_trade_units_(to_trade_units(period_ms, resolution)) {}

            template <typename TradeType, typename Candle>
            [[nodiscard]] bool should_trigger(const TradeType& trade, const Candle&) {
                if (reference_timestamp_ == 0) {
                    reference_timestamp_ = trade_timestamp(trade);
                }

                const bool trigger = timestamp_delta_reaches(trade_timestamp(trade), reference_timestamp_, static_cast<std::uint64_t>(period_in_trade_units_), false);
                
                if (trigger) {
                    reference_timestamp_ += period_in_trade_units_;
                }

                return trigger;
            }

        private:
            static std::int64_t to_trade_units(MillisecondPeriod period_ms, TimestampResolution resolution) {
                const std::uint64_t multiplier = resolution == TimestampResolution::Millisecond ? 1 :
                    resolution == TimestampResolution::Microsecond ? 1'000 : 1'000'000;

                if (period_ms.get() > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) / multiplier) {
                    throw std::overflow_error("time rule period does not fit in trade timestamp units");
                }
                
                return static_cast<std::int64_t>(period_ms.get() * multiplier);
            }

            std::int64_t reference_timestamp_;
            std::int64_t period_in_trade_units_;
    };
}
