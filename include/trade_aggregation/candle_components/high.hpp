#pragma once

#include "trade_aggregation/types.hpp"

#include <limits>

namespace trade_aggregation {

    class High {
        public:
            [[nodiscard]] double value() const noexcept { return high_; }
            void reset() noexcept { high_ = std::numeric_limits<double>::lowest(); }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept {
                if (trade_price(trade) > high_) {
                    high_ = trade_price(trade);
                }
            }

        private:
            double high_{};
    };
}
