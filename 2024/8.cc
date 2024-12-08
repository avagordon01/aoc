#include "util.hh"

auto main() -> int {
    using namespace util;
    namespace bp = boost::parser;

    const auto p_line = *(bp::char_ - bp::eol) >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(............
........0...
.....0......
.......0....
....0.......
......A.....
............
............
........A...
.........A..
............
............
)EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2024/8.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    const auto& lines = *result;

    //part 1
    using coord = Eigen::Vector2i;
    std::unordered_map<char, std::vector<coord>> antennas;
    for (const auto& [y, line]: lines | std::views::enumerate) {
        for (const auto [x, c]: line | std::views::enumerate) {
            const auto pos = coord{x, y};
            if (c == '.') {
                continue;
            }
            antennas[c].push_back(pos);
        }
    }
    const coord bounds = {lines[0].size(), lines.size()};
    const auto bounds_check = [&bounds](const auto& index) -> bool {
        return md_bounds_check(bounds, index);
    };
    std::unordered_set<coord> antinodes;
    for (const auto& [frequency, positions]: antennas) {
        for (const auto& a: positions) {
            for (const auto& b: positions) {
                if (b == a) {
                    continue;
                }
                const auto diff = b - a;
                const auto antinode_a = a - diff;
                const auto antinode_b = b + diff;
                if (bounds_check(antinode_a)) {
                    antinodes.insert(antinode_a);
                }
                if (bounds_check(antinode_b)) {
                    antinodes.insert(antinode_b);
                }
            }
        }
    }
    std::cout << "part 1 = " << antinodes.size() << std::endl;

    //part 2
    antinodes.clear();
    for (const auto& [frequency, positions]: antennas) {
        for (const auto& a: positions) {
            for (const auto& b: positions) {
                if (b == a) {
                    continue;
                }
                coord diff = b - a;
                coord antinode_a = a;
                while (bounds_check(antinode_a)) {
                    antinodes.insert(antinode_a);
                    antinode_a = antinode_a - diff;
                }
                coord antinode_b = b;
                while (bounds_check(antinode_b)) {
                    antinodes.insert(antinode_b);
                    antinode_b = antinode_b + diff;
                }
            }
        }
    }
    std::cout << "part 2 = " << antinodes.size() << std::endl;

    return 0;
}
