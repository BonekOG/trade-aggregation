#pragma once

#include "trade_aggregation/types.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace trade_aggregation {

    class MedianPrice {
        public:
            [[nodiscard]] double value() const {
                if (prices_.empty()) {
                    throw std::logic_error("median price is undefined for an empty candle");
                }

                std::vector<double> sorted_prices = prices_;

                if (std::any_of(sorted_prices.begin(), sorted_prices.end(), [](double price) { return std::isnan(price); })) {
                    throw std::logic_error("median price cannot order NaN values");
                }

                std::sort(sorted_prices.begin(), sorted_prices.end());
                
                return sorted_prices[sorted_prices.size() / 2];
            }

            void reset() noexcept { prices_.clear(); }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) { prices_.push_back(trade_price(trade)); }

        private:
            std::vector<double> prices_;
    };
}
