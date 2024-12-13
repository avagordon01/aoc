#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_line = (bp::ulong_long % bp::blank) >> bp::eol;
    const auto p_file = p_line >> bp::eoi;

    const auto result = bp::parse(file_as_string("2024/11.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& stones = *result;

    //NOTE enabling stone_map is ~10% faster at 2000 blinks
    //bottleneck is probably just unordered_map access atp
    //std::unordered_map<size_t, std::pair<size_t, std::optional<size_t>>> stone_map;
    std::unordered_map<size_t, size_t> stone_counts, stone_counts_next;


    for (const auto stone: stones) {
        stone_counts[stone]++;
    }

    const auto count = [](const auto& stone_counts) {
        return std::ranges::fold_left(stone_counts | std::views::values, 0, std::plus{});
    };

    const auto rules = [](const size_t stone) -> std::pair<size_t, std::optional<size_t>> {
        const auto digits = static_cast<size_t>(std::log10(stone)) + 1;
        if (stone == 0) {
            return {1, std::nullopt};
        } else if ((digits % 2) == 0) {
            const auto split = static_cast<size_t>(pow(10, digits / 2));
            const auto l = stone / split;
            const auto r = stone % split;
            return {l, r};
        } else {
            return {stone * 2024, std::nullopt};
        }
    };

    for (size_t blink = 0; blink < 75; blink++) {
        if (blink == 25) {
            std::cout << "part 1 = " << count(stone_counts) << std::endl;
        }
        stone_counts_next = stone_counts;
        for (const auto& [stone, count]: stone_counts) {
            /*
            if (!stone_map.contains(stone)) {
                stone_map[stone] = rules(stone);
            }
            const auto [l, r] = stone_map[stone];
            */
            const auto [l, r] = rules(stone);
            stone_counts_next[stone] -= count;
            stone_counts_next[l] += count;
            if (r) {
                stone_counts_next[*r] += count;
            }
        }
        std::swap(stone_counts_next, stone_counts);
    }
    std::cout << "part 2 = " << count(stone_counts) << std::endl;

    return 0;
}
