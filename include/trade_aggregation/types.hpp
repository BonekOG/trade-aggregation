#pragma once

#include <chrono>
#include <compare>
#include <concepts>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace trade_aggregation {

    enum class TimestampResolution {
        Millisecond,
        Microsecond,
        Nanosecond,
    };

    enum class By {
        Base,
        Quote,
    };

    struct Trade {
            std::int64_t timestamp{};
            double price{};
            double size{};

            friend bool operator==(const Trade&, const Trade&) = default;
    };

    namespace detail {

        template <typename T>
        concept HasTradeTimestampMethod = requires(const T& trade) {
            trade.timestamp();
        };

        template <typename T>
        concept HasTradePriceMethod = requires(const T& trade) {
            trade.price();
        };

        template <typename T>
        concept HasTradeSizeMethod = requires(const T& trade) {
            trade.size();
        };

        template <typename T>
        concept HasTradeTimestampResolutionMethod = requires(const T& trade) {
            trade.timestamp_resolution();
        };

        template <typename T>
        concept HasTradeTimestamp =
            (HasTradeTimestampMethod<T> && requires(const T& trade) {
                static_cast<std::int64_t>(trade.timestamp());
            }) ||
            (!HasTradeTimestampMethod<T> && requires(const T& trade) {
                static_cast<std::int64_t>(trade.timestamp);
            });

        template <typename T>
        concept HasTradePrice =
            (HasTradePriceMethod<T> && requires(const T& trade) {
                static_cast<double>(trade.price());
            }) ||
            (!HasTradePriceMethod<T> && requires(const T& trade) {
                static_cast<double>(trade.price);
            });

        template <typename T>
        concept HasTradeSize =
            (HasTradeSizeMethod<T> && requires(const T& trade) {
                static_cast<double>(trade.size());
            }) ||
            (!HasTradeSizeMethod<T> && requires(const T& trade) {
                static_cast<double>(trade.size);
            });

        template <typename T>
        concept HasValidTradeTimestampResolution =
            (HasTradeTimestampResolutionMethod<T> && requires(const T& trade) {
                { trade.timestamp_resolution() } -> std::convertible_to<TimestampResolution>;
            }) ||
            (!HasTradeTimestampResolutionMethod<T> && requires(const T& trade) {
                { trade.timestamp_resolution } -> std::convertible_to<TimestampResolution>;
            }) ||
            (!HasTradeTimestampResolutionMethod<T> && !requires(const T& trade) {
                trade.timestamp_resolution;
            });

    }

    template <typename T>
    [[nodiscard]] constexpr std::int64_t trade_timestamp(const T& trade) {
        if constexpr (requires { trade.timestamp(); }) {
            return static_cast<std::int64_t>(trade.timestamp());
        } else {
            return static_cast<std::int64_t>(trade.timestamp);
        }
    }

    template <typename T>
    [[nodiscard]] constexpr double trade_price(const T& trade) {
        if constexpr (requires { trade.price(); }) {
            return static_cast<double>(trade.price());
        } else {
            return static_cast<double>(trade.price);
        }
    }

    template <typename T>
    [[nodiscard]] constexpr double trade_size(const T& trade) {
        if constexpr (requires { trade.size(); }) {
            return static_cast<double>(trade.size());
        } else {
            return static_cast<double>(trade.size);
        }
    }

    template <typename T>
    [[nodiscard]] constexpr TimestampResolution trade_timestamp_resolution(const T& trade) {
        if constexpr (requires { trade.timestamp_resolution(); }) {
            return trade.timestamp_resolution();
        } else if constexpr (requires { trade.timestamp_resolution; }) {
            return trade.timestamp_resolution;
        } else {
            return TimestampResolution::Millisecond;
        }
    }

    template <typename T>
    concept TakerTrade = detail::HasTradeTimestamp<T> && detail::HasTradePrice<T> &&
                         detail::HasTradeSize<T> && detail::HasValidTradeTimestampResolution<T> &&
                         requires(const T& trade) {
                             { trade_timestamp(trade) } -> std::same_as<std::int64_t>;
                             { trade_price(trade) } -> std::same_as<double>;
                             { trade_size(trade) } -> std::same_as<double>;
                             { trade_timestamp_resolution(trade) } -> std::same_as<TimestampResolution>;
                         };

    [[nodiscard]] constexpr bool timestamp_delta_reaches(
        std::int64_t timestamp,
        std::int64_t reference_timestamp,
        std::uint64_t period,
        bool inclusive) noexcept {
        if (timestamp <= reference_timestamp) {
            return false;
        }
        const auto elapsed = static_cast<std::uint64_t>(timestamp) -
                             static_cast<std::uint64_t>(reference_timestamp);
        return inclusive ? elapsed >= period : elapsed > period;
    }

    class MillisecondPeriod {
        public:
            [[nodiscard]] static MillisecondPeriod from_non_zero(std::uint64_t millis) {
                if (millis == 0) {
                    throw std::invalid_argument("millisecond period must be non-zero");
                }
                return MillisecondPeriod(millis);
            }

            [[nodiscard]] static MillisecondPeriod from_non_zero_secs(std::uint64_t seconds) {
                if (seconds == 0 || seconds > std::numeric_limits<std::uint64_t>::max() / 1'000) {
                    throw std::invalid_argument("seconds must be non-zero and fit in milliseconds");
                }
                return MillisecondPeriod(seconds * 1'000);
            }

            [[nodiscard]] constexpr std::uint64_t get() const noexcept { return millis_; }

            friend constexpr bool operator==(MillisecondPeriod, MillisecondPeriod) = default;
            friend constexpr auto operator<=>(MillisecondPeriod, MillisecondPeriod) = default;

        private:
            explicit constexpr MillisecondPeriod(std::uint64_t millis) : millis_(millis) {}

            std::uint64_t millis_;
    };
}
