#pragma once

#include <concepts>

namespace trade_aggregation {

    template <typename Candle, typename TradeType>
    concept ModularCandleFor = std::copy_constructible<Candle> && std::default_initializable<Candle> &&
                               requires(Candle& candle, const TradeType& trade) {
                                   candle.update(trade);
                                   candle.reset();
                               };
}
