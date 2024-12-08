#include "util.hh"

auto main() -> int {
    //parse input
    namespace bp = boost::parser;

    const auto parser = *(bp::int_ >> bp::int_);

    const auto result = bp::parse(file_as_string("2024/1.input.txt"), parser, bp::ws);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    //part 1
    auto& pairs = *result;

    std::vector<int> left_side;
    std::vector<int> right_side;
    for (auto [l, r]: pairs) {
        left_side.push_back(l);
        right_side.push_back(r);
    }

    std::ranges::sort(left_side);
    std::ranges::sort(right_side);

    size_t total_distance = 0;
    for (auto [l, r]: std::views::zip(left_side, right_side)) {
        size_t distance = std::abs(l - r);
        total_distance += distance;
    }

    std::cout << "part 1 = " << total_distance << std::endl;

    //part 2
    std::unordered_map<int, int> counts;
    for (auto n: right_side) {
        counts[n]++;
    }

    size_t total_similarity_score = 0;
    for (auto l: left_side) {
        const auto count = counts[l];
        const auto similarity_score = count * l;
        total_similarity_score += similarity_score;
    }

    std::cout << "part 2 = " << total_similarity_score << std::endl;

    return 0;
}
