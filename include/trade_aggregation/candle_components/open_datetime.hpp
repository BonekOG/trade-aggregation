#pragma once

#include "trade_aggregation/types.hpp"

#include <chrono>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace trade_aggregation {

    class OpenDateTime {
        public:
            using Value = std::chrono::sys_time<std::chrono::nanoseconds>;

            [[nodiscard]] Value value() const noexcept { return value_; }
            void reset() noexcept { init_ = true; }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) {
                if (init_) {
                    value_ = to_utc_time(trade_timestamp(trade), trade_timestamp_resolution(trade));
                    init_ = false;
                }
            }

        private:
            static Value to_utc_time(std::int64_t timestamp, TimestampResolution resolution) {
                const std::int64_t multiplier = resolution == TimestampResolution::Millisecond ? 1'000'000 : resolution == TimestampResolution::Microsecond ? 1'000
                                                                                                                                                            : 1;
                if (timestamp > std::numeric_limits<std::int64_t>::max() / multiplier ||
                    timestamp < std::numeric_limits<std::int64_t>::min() / multiplier) {
                    throw std::overflow_error("trade timestamp cannot be represented as nanoseconds");
                }
                
                return Value(std::chrono::nanoseconds(timestamp * multiplier));
            }

            bool init_{true};
            Value value_{};
    };
}
