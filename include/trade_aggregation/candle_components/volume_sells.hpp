#pragma once

#include "trade_aggregation/types.hpp"

#include <cmath>

namespace trade_aggregation {

    class VolumeSells {
        public:
            [[nodiscard]] double value() const noexcept { return sell_volume_; }
            void reset() noexcept { sell_volume_ = 0.0; }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept {
                const double size = trade_size(trade);
                if (std::signbit(size)) {
                    sell_volume_ += std::abs(size);
                }
            }

        private:
            double sell_volume_{};
    };
}
