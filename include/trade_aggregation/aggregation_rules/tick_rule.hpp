#pragma once

#include "trade_aggregation/aggregation_rule.hpp"

#include <cstddef>

namespace trade_aggregation {

    class TickRule {
        public:
            explicit TickRule(std::size_t n_ticks) : init_(true), tick_counter_(0), n_ticks_(n_ticks) {}

            template <typename TradeType, typename Candle>
            [[nodiscard]] bool should_trigger(const TradeType&, const Candle&) {
                if (init_) {
                    tick_counter_ = 0;
                    init_ = false;
                }

                ++tick_counter_;

                if (tick_counter_ >= n_ticks_) {
                    init_ = true;

                    return init_;
                }
                
                return false;
            }

        private:
            bool init_;
            std::size_t tick_counter_;
            std::size_t n_ticks_;
    };
}
