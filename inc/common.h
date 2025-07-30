#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <cstdint>


uint64_t parse_size_to_bytes(const std::string& input) {
    std::string trimmed;
    for (char c : input) {
        if (!std::isspace(static_cast<unsigned char>(c))) trimmed += c;
    }

    size_t idx = 0;
    while (idx < trimmed.size() && (std::isdigit(trimmed[idx]) || trimmed[idx] == '.')) {
        ++idx;
    }

    if (idx == 0) throw std::invalid_argument("Invalid input: " + input);

    double value = std::stod(trimmed.substr(0, idx));
    std::string unit = trimmed.substr(idx);

    static const std::unordered_map<std::string, uint64_t> unit_table = {
        {"b", 1ULL},
        {"k", 1ULL << 10}, {"kb", 1ULL << 10},
        {"m", 1ULL << 20}, {"mb", 1ULL << 20},
        {"g", 1ULL << 30}, {"gb", 1ULL << 30},
        {"t", 1ULL << 40}, {"tb", 1ULL << 40},
        {"kib", 1ULL << 10}, {"mib", 1ULL << 20},
        {"gib", 1ULL << 30}, {"tib", 1ULL << 40},
        {"B", 1ULL}, {"K", 1ULL << 10}, {"M", 1ULL << 20}, {"G", 1ULL << 30}, {"T", 1ULL << 40},
        {"KB", 1ULL << 10}, {"MB", 1ULL << 20}, {"GB", 1ULL << 30}, {"TB", 1ULL << 40},
        {"KiB", 1ULL << 10}, {"MiB", 1ULL << 20}, {"GiB", 1ULL << 30}, {"TiB", 1ULL << 40}
    };

    auto it = unit_table.find(unit);
    if (it == unit_table.end()) {
        throw std::invalid_argument("Unknown unit: " + unit);
    }

    return static_cast<uint64_t>(std::round(value * it->second));
}