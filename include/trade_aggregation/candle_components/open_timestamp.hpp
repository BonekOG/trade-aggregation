#pragma once

#include "trade_aggregation/types.hpp"

#include <cstdint>

namespace trade_aggregation {

    class OpenTimeStamp {
        public:
            [[nodiscard]] std::int64_t value() const noexcept { return value_; }
            void reset() noexcept { init_ = true; }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept {
                if (init_) {
                    value_ = trade_timestamp(trade);
                    init_ = false;
                }
            }

        private:
            bool init_{true};
            std::int64_t value_{};
    };
}
