#pragma once

#include "trade_aggregation/types.hpp"

#include <cmath>
#include <cstddef>

namespace trade_aggregation {

    class Entropy {
        public:
            [[nodiscard]] double value() const noexcept {
                const double probability_buy = static_cast<double>(buys_) / static_cast<double>(total_observed_trades_);
                const double probability_sell = 1.0 - probability_buy;

                double entropy = probability_buy * std::log2(probability_buy) +
                                 probability_sell * std::log2(probability_sell);

                if (std::isnan(entropy)) {
                    entropy = 0.0;
                }

                return -entropy;
            }

            void reset() noexcept {
                buys_ = 0;
                total_observed_trades_ = 0;
            }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept {
                if (trade_size(trade) > 0.0) {
                    ++buys_;
                }
                
                ++total_observed_trades_;
            }

        private:
            std::size_t buys_{};
            std::size_t total_observed_trades_{};
    };
}
