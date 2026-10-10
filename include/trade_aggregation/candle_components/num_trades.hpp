#pragma once

#include "trade_aggregation/types.hpp"

#include <concepts>
#include <cstdint>

namespace trade_aggregation {

    template <std::unsigned_integral Count = std::uint32_t>
    class NumTrades {
        public:
            [[nodiscard]] Count value() const noexcept { return value_; }
            void reset() noexcept { value_ = 0; }

            template <TakerTrade TradeType>
            void update(const TradeType&) noexcept { ++value_; }

        private:
            Count value_{};
    };
}
