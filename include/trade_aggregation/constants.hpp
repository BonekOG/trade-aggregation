#pragma once

#include "trade_aggregation/types.hpp"

namespace trade_aggregation {
    inline const MillisecondPeriod M1 = MillisecondPeriod::from_non_zero_secs(60);
    inline const MillisecondPeriod M5 = MillisecondPeriod::from_non_zero_secs(300);
    inline const MillisecondPeriod M15 = MillisecondPeriod::from_non_zero_secs(900);
    inline const MillisecondPeriod M30 = MillisecondPeriod::from_non_zero_secs(1'800);
    inline const MillisecondPeriod H1 = MillisecondPeriod::from_non_zero_secs(3'600);
    inline const MillisecondPeriod H2 = MillisecondPeriod::from_non_zero_secs(7'200);
    inline const MillisecondPeriod H4 = MillisecondPeriod::from_non_zero_secs(14'400);
    inline const MillisecondPeriod H8 = MillisecondPeriod::from_non_zero_secs(28'800);
    inline const MillisecondPeriod H12 = MillisecondPeriod::from_non_zero_secs(43'200);
    inline const MillisecondPeriod D1 = MillisecondPeriod::from_non_zero_secs(86'400);
}
