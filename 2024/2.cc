#include <boost/parser/parser.hpp>
#include <boost/hana.hpp>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ranges>
#include <cmath>
#include "util.hh"

auto main() -> int {
    //parse input
    namespace bp = boost::parser;

    auto parser = ((bp::long_long % ' ') % '\n') >> bp::ws;

    auto result = bp::parse(file_as_string("2024/2.input.txt"), parser);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    //part 1
    const auto& reports = *result;

    auto level_check = [](auto t) {
        auto [a, b] = t;
        return std::abs(b - a) >= 1 && std::abs(b - a) <= 3;
    };
    auto sign_check = [](auto t) {
        auto [a, b, c] = t;
        return sign(b - a) == sign(c - b);
    };

    auto all_safe = [&](auto&& r) {
        auto all_level_safe = std::ranges::all_of(
            r | std::views::adjacent<2>,
            level_check);
        auto all_sign_safe = std::ranges::all_of(
            r | std::views::adjacent<3>,
            sign_check);
        return all_level_safe && all_sign_safe;
    };

    size_t num_safe = 0;
    for (const auto& report: reports) {
        if (all_safe(report)) {
            num_safe++;
        }
    }
    std::cout << "part 1 = " << num_safe << std::endl;

    //part 2
    size_t num_safe_2 = 0;
    for (const auto& report: reports) {
        if (all_safe(report)) {
            num_safe_2++;
            continue;
        }
        for (size_t skip = 0; skip < report.size(); skip++) {
            const auto skip_nth =
                std::views::enumerate
                | std::views::filter([&](auto&& pair) {
                        return std::get<0>(pair) != static_cast<unsigned int>(skip);
                })
                | std::views::values;
            auto skipped = report | skip_nth;
            if (all_safe(skipped)) {
                num_safe_2++;
                break;
            }
        }
    }
    std::cout << "part 2 = " << num_safe_2 << std::endl;

    return 0;
}
