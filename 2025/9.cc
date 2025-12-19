#include "util.hh"
#include <boost/parser/parser.hpp>

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line = bp::long_ >> ',' >> bp::long_ >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(7,1
11,1
11,7
9,7
9,5
2,5
2,3
7,3
)EOF"};
    // const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string(binary_name_to_test_file_name(argv[0])), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& input = *result;

    //part 1
    std::vector<std::tuple<size_t, size_t, size_t>> areas;
    int64_t max_area = 0;
    for (size_t i = 0; i < input.size(); i++) {
        for (size_t j = i + 1; j < input.size(); j++) {
            const auto& [a_x, a_y] = input[i];
            const auto& [b_x, b_y] = input[j];
            const auto area = (std::abs(b_x - a_x) + 1) * (std::abs(b_y - a_y) + 1);
            max_area = std::max(max_area, area);
            areas.push_back({area, i, j});
        }
    }
    std::cout << "part 1 = " << max_area << std::endl;

    //part 2
    int64_t w = 100000;
    int64_t h = 100000;
    std::vector<int64_t> min_xs(h, w);
    std::vector<int64_t> max_xs(h, 0);
    for (size_t i = 0; i < input.size(); i++) {
        const auto& [ax, ay] = input[i];

        const auto& [bx, by] = input[(i + 1) % input.size()];

        const auto dx = sign(bx - ax);
        const auto dy = sign(by - ay);

        for (int64_t x = ax, y = ay; x != bx || y != by; x += dx, y += dy) {
            min_xs[y] = std::min(min_xs[y], x);
            max_xs[y] = std::max(max_xs[y], x);
        }
    }
    for (int64_t y = 0; y < h; y++) {
        const auto min = min_xs[y];
        const auto max = max_xs[y];
        if (min == 0 || max == w) {
            continue;
        }
    }
    std::ranges::sort(areas, std::greater(), [](const auto t){
        const auto [area, i, j] = t;
        return area;
    });
    for (const auto& [area, i, j]: areas) {
        const auto [ax, ay] = input[i];
        const auto [bx, by] = input[j];
        bool contained = true;
        for (int64_t y = std::min(ay, by); y < std::max(ay, by); y++) {
            const auto min = min_xs[y];
            const auto max = max_xs[y];
            if (ax >= min && ax <= max && bx >= min && bx <= max) {
            } else {
                contained = false;
                break;
            }
        }
        if (contained) {
            std::cout << "part 2 = " << area << std::endl;
            break;
        }
    }

    return 0;
}
