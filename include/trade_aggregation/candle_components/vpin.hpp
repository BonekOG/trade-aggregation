#pragma once

#include "trade_aggregation/types.hpp"

#include <stdexcept>

namespace trade_aggregation {

    class Vpin {
        public:
            [[nodiscard]] double value() const {
                throw std::logic_error("VPIN is not implemented in the upstream trade_aggregation-rs project");
            }

            void reset() {
                throw std::logic_error("VPIN is not implemented in the upstream trade_aggregation-rs project");
            }

            template <TakerTrade TradeType>
            void update(const TradeType&) {
                throw std::logic_error("VPIN is not implemented in the upstream trade_aggregation-rs project");
            }
    };
}
