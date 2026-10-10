#pragma once

#include "trade_aggregation/types.hpp"

#include <cstdint>

namespace trade_aggregation {

    class CloseTimeStamp {
        public:
            [[nodiscard]] std::int64_t value() const noexcept { return value_; }
            void reset() noexcept {}

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept { value_ = trade_timestamp(trade); }

        private:
            std::int64_t value_{};
    };
}
