#pragma once

#include "trade_aggregation/candle_components/candle_component.hpp"

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace trade_aggregation {

    template <typename... Components>
    class Candle {
        public:
            Candle() = default;

            template <TakerTrade TradeType>
                requires(CandleComponentFor<Components, TradeType> && ...)
            void update(const TradeType& trade) {
                update_components(trade, std::make_index_sequence<sizeof...(Components)>{});
            }

            void reset() {
                reset_components(std::make_index_sequence<sizeof...(Components)>{});
            }

            template <typename Component>
            [[nodiscard]] const Component& component() const {
                return std::get<Component>(components_);
            }

            template <std::size_t Index>
            [[nodiscard]] const std::tuple_element_t<Index, std::tuple<Components...>>& component_at() const {
                return std::get<Index>(components_);
            }

        private:
            template <TakerTrade TradeType, std::size_t... Indices>
            void update_components(const TradeType& trade, std::index_sequence<Indices...>) {
                (std::get<Indices>(components_).update(trade), ...);
            }

            template <std::size_t... Indices>
            void reset_components(std::index_sequence<Indices...>) {
                (std::get<Indices>(components_).reset(), ...);
            }

            std::tuple<Components...> components_{};
    };
}
