#pragma once

#include "trade_aggregation/types.hpp"

#include <cstdint>

namespace trade_aggregation {

    class TimeVelocity {
        public:
            [[nodiscard]] double value() const noexcept {
                const auto elapsed = last_time_ > init_time_ ? static_cast<std::uint64_t>(last_time_) - static_cast<std::uint64_t>(init_time_) : 0;
                const double elapsed_seconds = static_cast<double>(elapsed < 1 ? 1 : elapsed);
                
                return 1.0 / elapsed_seconds;
            }

            void reset() noexcept { init_ = true; }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept {
                const std::int64_t divisor = trade_timestamp_resolution(trade) == TimestampResolution::Millisecond ? 1'000 : trade_timestamp_resolution(trade) == TimestampResolution::Microsecond ? 1'000'000
                                                                                                                                                                                                   : 1'000'000'000;
                if (init_) {
                    init_time_ = trade_timestamp(trade) / divisor;
                    init_ = false;
                }

                last_time_ = trade_timestamp(trade) / divisor;
            }

        private:
            bool init_{true};
            std::int64_t init_time_{};
            std::int64_t last_time_{};
    };
}
