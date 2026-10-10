#pragma once

#include "trade_aggregation/types.hpp"

#include <cmath>

namespace trade_aggregation {

    class DirectionalVolumeRatio {
        public:
            [[nodiscard]] double value() const noexcept { return buy_volume_ / volume_; }
            void reset() noexcept {
                volume_ = 0.0;
                buy_volume_ = 0.0;
            }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept {
                volume_ += std::abs(trade_size(trade));
                
                if (trade_size(trade) > 0.0) {
                    buy_volume_ += trade_size(trade);
                }
            }

        private:
            double volume_{};
            double buy_volume_{};
    };
}
