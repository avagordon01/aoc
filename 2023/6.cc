#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_times = "Time:" >> *bp::ulong_long;
    const auto p_distances = "Distance:" >> *bp::ulong_long;
    const auto file = p_times >> p_distances;

    const auto result = bp::parse(file_as_string("2023/6.input.txt"), file, bp::ws);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    const auto& [times, distances] = *result;

    size_t ways_product = 1;
    for (auto [race_time, record_distance]: std::views::zip(times, distances)) {
        size_t ways = 0;
        for (size_t i = 0; i <= race_time; i++) {
            size_t distance = (race_time - i) * i;
            if (distance > record_distance) {
                ways++;
            }
        }
        ways_product *= ways;
    }
    std::cout << "part 1 = " << ways_product << std::endl;

    //TODO parse
    size_t race_time = 59796575;
    size_t record_distance = 597123410321328;
    size_t ways = 0;
    for (size_t i = 0; i <= race_time; i++) {
        size_t distance = (race_time - i) * i;
        if (distance > record_distance) {
            ways++;
        }
    }
    std::cout << "part 2 = " << ways << std::endl;

    return 0;
}
