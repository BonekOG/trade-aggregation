#pragma once

#include "trade_aggregation/types.hpp"
#include "trade_aggregation/welford_online.hpp"

namespace trade_aggregation {

    class StdDevPrices {
        public:
            [[nodiscard]] double value() const noexcept { return welford_.std_dev(); }
            void reset() noexcept { welford_.reset(); }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept { welford_.add(trade_price(trade)); }

        private:
            WelfordOnline welford_;
    };
}
