#include "trade_aggregation/utils.hpp"

#include <charconv>
#include <cmath>
#include <fstream>
#include <ios>
#include <istream>
#include <limits>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

using namespace std;

namespace trade_aggregation {
    namespace {

        Result<optional<vector<string>>> read_csv_record(istream& input, string& line, size_t& line_number) {
            vector<string> fields;
            string field;
            bool in_quotes = false;

            while (getline(input, line)) {
                ++line_number;
                const bool has_carriage_return = !line.empty() && line.back() == '\r';

                if (has_carriage_return) {
                    line.pop_back();
                }

                if (!in_quotes && line.empty()) {
                    continue;
                }

                for (size_t index = 0; index < line.size(); ++index) {
                    const char character = line[index];
                    if (character == '"') {
                        if (in_quotes && index + 1 < line.size() && line[index + 1] == '"') {
                            field.push_back('"');
                            ++index;
                        } else {
                            in_quotes = !in_quotes;
                        }
                    } else if (character == ',' && !in_quotes) {
                        fields.push_back(move(field));
                        field.clear();
                    } else {
                        field.push_back(character);
                    }
                }

                if (in_quotes) {
                    if (has_carriage_return) {
                        field.push_back('\r');
                    }
                    field.push_back('\n');
                    continue;
                }

                fields.push_back(move(field));
                return optional<vector<string>>(move(fields));
            }

            if (input.bad()) {
                return Error(ErrorCode::Io, "failed while reading CSV file on line " + to_string(line_number));
            }

            if (in_quotes) {
                return Error(ErrorCode::Csv, "unterminated quoted field on CSV line " + to_string(line_number));
            }

            return optional<vector<string>>{};
        }

        bool parse_int64(const string& text, int64_t& value) {
            const char* begin = text.data();
            const char* end = begin + text.size();

            if (begin != end && *begin == '+') {
                ++begin;
                if (begin != end && (*begin == '+' || *begin == '-')) {
                    return false;
                }
            }

            const auto result = from_chars(begin, end, value);

            return result.ec == errc{} && result.ptr == end;
        }

        bool parse_double(const string& text, double& value) {
            const char* begin = text.data();
            const char* end = begin + text.size();

            if (begin != end && *begin == '+') {
                ++begin;
                if (begin != end && (*begin == '+' || *begin == '-')) {
                    return false;
                }
            }

            const auto result = from_chars(begin, end, value, chars_format::general);

            return result.ec == errc{} && result.ptr == end;
        }

    }

    double candle_volume_from_time_period(double total_volume, double total_time_days, double target_time_minutes) noexcept {
        const double number_of_candles = total_time_days * 24.0 * (60.0 / target_time_minutes);
        
        return total_volume / number_of_candles;
    }

    Result<vector<Trade>> load_trades_from_csv(string_view filename) {
        const string path(filename);
        ifstream file(path, ios::binary);

        if (!file) {
            return Error(ErrorCode::Io, "could not open CSV file: " + path);
        }

        vector<Trade> trades;
        string line;
        size_t line_number = 0;

        auto parsed_header = read_csv_record(file, line, line_number);

        if (!parsed_header) {
            return parsed_header.error();
        }

        if (!parsed_header.value()) {
            return Error(ErrorCode::Csv, "CSV file is empty: " + path);
        }

        const size_t expected_column_count = parsed_header.value()->size();

        while (true) {
            auto parsed = read_csv_record(file, line, line_number);
            if (!parsed) {
                return parsed.error();
            }

            if (!parsed.value()) {
                break;
            }

            const auto& row = *parsed.value();

            if (row.size() != expected_column_count) {
                return Error(ErrorCode::Csv, "inconsistent column count on CSV line " + to_string(line_number));
            }

            if (row.size() < 3) {
                return Error(ErrorCode::Csv, "expected at least three columns on CSV line " + to_string(line_number));
            }

            Trade trade;
            if (!parse_int64(row[0], trade.timestamp)) {
                return Error(ErrorCode::ParseInt, "invalid timestamp on CSV line " + to_string(line_number));
            }

            if (!parse_double(row[1], trade.price)) {
                return Error(ErrorCode::ParseFloat, "invalid price on CSV line " + to_string(line_number));
            }

            if (!parse_double(row[2], trade.size)) {
                return Error(ErrorCode::ParseFloat, "invalid size on CSV line " + to_string(line_number));
            }

            trades.push_back(trade);
        }

        return trades;
    }
}
