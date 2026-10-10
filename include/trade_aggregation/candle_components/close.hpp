#pragma once

#include "trade_aggregation/types.hpp"

namespace trade_aggregation {

    class Close {
        public:
            [[nodiscard]] double value() const noexcept { return value_; }
            void reset() noexcept {}

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept { value_ = trade_price(trade); }

        private:
            double value_{};
    };
}
