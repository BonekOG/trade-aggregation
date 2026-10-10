#pragma once

#include "trade_aggregation/types.hpp"

#include <cstddef>

namespace trade_aggregation {

    class DirectionalTradeRatio {
        public:
            [[nodiscard]] double value() const noexcept {
                return static_cast<double>(num_buys_) / static_cast<double>(num_trades_);
            }
            void reset() noexcept {
                num_buys_ = 0;
                num_trades_ = 0;
            }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept {
                ++num_trades_;
                
                if (trade_size(trade) > 0.0) {
                    ++num_buys_;
                }
            }

        private:
            std::size_t num_buys_{};
            std::size_t num_trades_{};
    };
}
