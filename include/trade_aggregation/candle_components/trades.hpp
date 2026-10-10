#pragma once

#include "trade_aggregation/types.hpp"

#include <concepts>
#include <vector>

namespace trade_aggregation {

    template <TakerTrade TradeType>
        requires std::copy_constructible<TradeType>
    class Trades {
        public:
            [[nodiscard]] std::vector<TradeType> value() const { return trades_; }
            void reset() noexcept { trades_.clear(); }

            void update(const TradeType& trade) { trades_.push_back(trade); }

        private:
            std::vector<TradeType> trades_;
    };
}
