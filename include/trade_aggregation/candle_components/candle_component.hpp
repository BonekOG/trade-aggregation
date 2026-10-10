#pragma once

#include "trade_aggregation/types.hpp"

#include <concepts>

namespace trade_aggregation {

    template <typename Component, typename Value>
    concept CandleComponent = requires(Component& component, const Component& const_component) {
        { const_component.value() } -> std::convertible_to<Value>;
        component.reset();
    };

    template <typename Component, typename TradeType>
    concept CandleComponentUpdate = TakerTrade<TradeType> && requires(
                                                                 Component& component, const TradeType& trade) {
        component.update(trade);
    };

    template <typename Component, typename TradeType>
    concept CandleComponentFor = CandleComponentUpdate<Component, TradeType> && requires(
                                                                                    Component& component, const Component& const_component) {
        const_component.value();
        component.reset();
    };
}
