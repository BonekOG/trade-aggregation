#pragma once

#include "trade_aggregation/types.hpp"

#include <cmath>

namespace trade_aggregation {

    class VolumeBuys {
        public:
            [[nodiscard]] double value() const noexcept { return buy_volume_; }
            void reset() noexcept { buy_volume_ = 0.0; }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept {
                const double size = trade_size(trade);
                if (!std::signbit(size)) {
                    buy_volume_ += std::abs(size);
                }
            }

        private:
            double buy_volume_{};
    };
}
