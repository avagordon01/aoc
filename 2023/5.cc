#include "util.hh"

struct interval_map {
    uint64_t dest;
    uint64_t src;
    uint64_t size;
};

struct interval {
    uint64_t start;
    uint64_t size;
};

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_vec_to_struct = [](const std::vector<unsigned long long>& v) {
        return interval_map{v[0], v[1], v[2]};
    };

    const auto p_seeds = bp::lit("seeds:") >> *bp::ulong_long;
    const auto p_words =
        bp::lit("seed") |
        bp::lit("soil") |
        bp::lit("fertilizer") |
        bp::lit("water") |
        bp::lit("light") |
        bp::lit("temperature") |
        bp::lit("humidity") |
        bp::lit("location");
    const auto p_map = 
        p_words >> bp::lit("-to-") >> p_words >> bp::lit("map:") >>
        *(bp::transform(p_vec_to_struct)[bp::repeat(3)[bp::ulong_long]]);
    const auto p_file = p_seeds >> (*p_map);

    std::string example = R"EOF(seeds: 79 14 55 13

seed-to-soil map:
50 98 2
52 50 48

soil-to-fertilizer map:
0 15 37
37 52 2
39 0 15

fertilizer-to-water map:
49 53 8
0 11 42
42 0 7
57 7 4

water-to-light map:
88 18 7
18 25 70

light-to-temperature map:
45 77 23
81 45 19
68 64 13

temperature-to-humidity map:
0 69 1
1 0 69

humidity-to-location map:
60 56 37
56 93 4
)EOF";

    auto convert = [](auto num, const std::vector<interval_map>& map) -> decltype(num) {
        for (const auto& range: map) {
            if (num >= range.src && num < range.src + range.size) {
                return (num - range.src) + range.dest;
            }
        }
        return num;
    };

    const auto result = bp::parse(file_as_string("2023/5.input.txt"), p_file, bp::ws);
    //const auto result = bp::parse(example, file, bp::ws);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    auto [seeds, maps] = *result;

    //part 1
    uint64_t lowest_location = std::numeric_limits<uint64_t>::max();
    for (const auto seed: seeds) {
        uint64_t x = seed;
        for (const auto& map: maps) {
            x = convert(x, map);
        }
        uint64_t location = x;
        lowest_location = std::min(lowest_location, location);
    }
    std::cout << "part 1 = " << lowest_location << std::endl;

    auto convert_interval = [](std::vector<interval>& intervals, const std::vector<interval_map>& interval_map) {
        size_t i = 0;
        size_t j = 0;
        while (i < intervals.size()) {
            auto& interval = intervals[i];
        }
    };

    //part 2
    for (auto& map: maps) {
        std::sort(map.begin(), map.end(), [](const auto& a, const auto& b) {
            return a.src < b.src;
        });
        for (size_t i = 0; i < map.size() - 1; i++) {
            const auto& range_a = map[i];
            const auto& range_b = map[i + 1];
            if (range_a.src + range_a.size <= range_b.src) {
            } else {
                std::cerr << "overlapping ranges" << std::endl;
                return 1;
            }
        }
        std::sort(map.begin(), map.end(), [](const auto& a, const auto& b) {
            return a.dest < b.dest;
        });
        for (size_t i = 0; i < map.size() - 1; i++) {
            const auto& range_a = map[i];
            const auto& range_b = map[i + 1];
            if (range_a.dest + range_a.size <= range_b.dest) {
            } else {
                std::cerr << "overlapping ranges" << std::endl;
                return 1;
            }
        }
    }
    {
        uint64_t lowest_location = std::numeric_limits<uint64_t>::max();
        for (size_t i = 0; i < seeds.size(); i += 2) {
            const auto start = seeds[i];
            const auto length = seeds[i+1];
            for (auto seed = start; seed < start + length; seed++) {
                uint64_t x = seed;
                for (const auto& map: maps) {
                    x = convert(x, map);
                }
                uint64_t location = x;
                lowest_location = std::min(lowest_location, location);
            }
            if (i % 1024 == 0) {
                std::cout << "%f" << 100.0f * static_cast<float>(i) / seeds.size() << std::endl;
            }
        }
        std::cout << "part 2 = " << lowest_location << std::endl;
    }

    return 0;
}
