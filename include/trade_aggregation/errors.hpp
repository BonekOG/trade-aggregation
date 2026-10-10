#pragma once

#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

namespace trade_aggregation {

    enum class ErrorCode {
        Io,
        Csv,
        ParseInt,
        ParseFloat,
        InvalidParam,
    };

    class Error {
        public:
            Error(ErrorCode code, std::string message) : code_(code), message_(std::move(message)) {}

            [[nodiscard]] ErrorCode code() const noexcept { return code_; }
            [[nodiscard]] const std::string& message() const noexcept { return message_; }

        private:
            ErrorCode code_;
            std::string message_;
    };

    template <typename T>
    class Result {
        public:
            Result(T value) : value_(std::move(value)) {}
            Result(Error error) : value_(std::move(error)) {}

            [[nodiscard]] bool has_value() const noexcept { return std::holds_alternative<T>(value_); }
            [[nodiscard]] explicit operator bool() const noexcept { return has_value(); }

            T& value() & {
                if (!has_value()) {
                    throw std::logic_error(std::get<Error>(value_).message());
                }

                return std::get<T>(value_);
            }

            const T& value() const& {
                if (!has_value()) {
                    throw std::logic_error(std::get<Error>(value_).message());
                }

                return std::get<T>(value_);
            }

            T&& value() && {
                if (!has_value()) {
                    throw std::logic_error(std::get<Error>(value_).message());
                }
                
                return std::move(std::get<T>(value_));
            }

            [[nodiscard]] Error& error() & { return std::get<Error>(value_); }
            [[nodiscard]] const Error& error() const& { return std::get<Error>(value_); }

        private:
            std::variant<T, Error> value_;
    };
}
