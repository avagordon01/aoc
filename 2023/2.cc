#include <boost/parser/parser.hpp>
#include <boost/hana.hpp>
#include <vector>
#include <unordered_map>
#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    auto rgb_list_to_map = [&](const std::vector<std::tuple<int, std::string>>& rgbs) {
        std::unordered_map<std::string, int> rgb;
        for (auto [num, colour]: rgbs) {
            rgb[colour] = num;
        }
        return rgb;
    };

    const auto rgb_list = (bp::int_ >> (
        bp::string("red") |
        bp::string("green") |
        bp::string("blue")
    )) % ',';

    const auto turn = bp::transform(rgb_list_to_map)[rgb_list];
    const auto game = bp::lit("Game") >> bp::int_ >> ':' >> (turn % ';');
    const auto games = *game;

    const auto result = bp::parse(file_as_string("2023/2.input.txt"), games, bp::ws);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    //part 1
    auto turn_possible = [](const std::unordered_map<std::string, int>& turn) -> bool {
        return get_or(turn, "red", 0) <= 12 &&
            get_or(turn, "green", 0) <= 13 &&
            get_or(turn, "blue", 0) <= 14;
    };
    size_t id_sum = 0;
    for (const auto& game: *result) {
        const auto& [id, turns] = game;
        bool possible = true;
        for (const auto& turn: turns) {
            if (!turn_possible(turn)) {
                possible = false;
            }
        }
        if (possible) {
            id_sum += id;
        }
    }
    std::cout << "part 1 = " << id_sum << std::endl;

    //part 2
    size_t power_sum = 0;
    for (const auto& game: *result) {
        const auto& [id, turns] = game;
        std::unordered_map<std::string, int> min_cubes = {
            {"red", 0},
            {"green", 0},
            {"blue", 0},
        };
        for (const auto& turn: turns) {
            for (auto& [colour, num]: min_cubes) {
                num = std::max(num, get_or(turn, colour, 0));
            }
        }
        size_t power = min_cubes["red"] * min_cubes["green"] * min_cubes["blue"];
        power_sum += power;
    }
    std::cout << "part 2 = " << power_sum << std::endl;

    return 0;
}
