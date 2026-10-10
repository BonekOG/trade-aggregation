#pragma once

#include "trade_aggregation/types.hpp"

#include <cmath>

namespace trade_aggregation {

    class Volume {
        public:
            [[nodiscard]] double value() const noexcept { return volume_; }
            void reset() noexcept { volume_ = 0.0; }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept { volume_ += std::abs(trade_size(trade)); }

        private:
            double volume_{};
    };
}
