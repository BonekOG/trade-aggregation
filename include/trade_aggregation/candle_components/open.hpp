#pragma once

#include "trade_aggregation/types.hpp"

namespace trade_aggregation {

    class Open {
        public:
            [[nodiscard]] double value() const noexcept { return value_; }
            void reset() noexcept { init_ = true; }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept {
                if (init_) {
                    value_ = trade_price(trade);
                    init_ = false;
                }
            }

        private:
            bool init_{true};
            double value_{};
    };
}
