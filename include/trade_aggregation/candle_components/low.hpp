#pragma once

#include "trade_aggregation/types.hpp"

#include <limits>

namespace trade_aggregation {

    class Low {
        public:
            [[nodiscard]] double value() const noexcept { return low_; }
            void reset() noexcept { low_ = std::numeric_limits<double>::max(); }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept {
                if (trade_price(trade) < low_) {
                    low_ = trade_price(trade);
                }
            }

        private:
            double low_{std::numeric_limits<double>::max()};
    };
}
