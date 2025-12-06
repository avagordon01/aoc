#include "util.hh"

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line = *bp::char_(".#") >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(...#......
.......#..
#.........
..........
......#...
.#........
.........#
..........
.......#..
#...#.....
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
    std::vector<bool> empty_rows(input.size(), true);
    std::vector<bool> empty_cols(input[0].size(), true);;
    std::vector<std::pair<size_t, size_t>> positions;
    for (size_t y = 0; y < input.size(); y++) {
        for (size_t x = 0; x < input[0].size(); x++) {
            const auto c = input[y][x];
            if (c == '#') {
                empty_rows[y] = false;
                empty_cols[x] = false;
                positions.push_back({x, y});
            }
        }
    }
    const auto count_cols_between = [empty_cols](size_t scale, size_t x0, size_t x1) {
        if (x0 > x1) {
            std::swap(x0, x1);
        }
        size_t diff = x1 - x0;
        for (size_t x = x0; x < x1; x++) {
            diff += scale * empty_cols[x];
        }
        return diff;
    };
    const auto count_rows_between = [empty_rows](size_t scale, size_t y0, size_t y1) {
        if (y0 > y1) {
            std::swap(y0, y1);
        }
        size_t diff = y1 - y0;
        for (size_t y = y0; y < y1; y++) {
            diff += scale * empty_rows[y];
        }
        return diff;
    };
    size_t scale = 1;
    size_t sum = 0;
    for (size_t i = 0; i < positions.size(); i++) {
        for (size_t j = i + 1; j < positions.size(); j++) {
            const auto [x0, y0] = positions[i];
            const auto [x1, y1] = positions[j];
            size_t distance = count_cols_between(scale, x0, x1) + count_rows_between(scale, y0, y1);
            sum += distance;
        }
    }
    std::cout << "part 1 = " << sum << std::endl;

    //part 2
    scale = 999999;
    sum = 0;
    for (size_t i = 0; i < positions.size(); i++) {
        for (size_t j = i + 1; j < positions.size(); j++) {
            const auto [x0, y0] = positions[i];
            const auto [x1, y1] = positions[j];
            size_t distance = count_cols_between(scale, x0, x1) + count_rows_between(scale, y0, y1);
            sum += distance;
        }
    }
    std::cout << "part 2 = " << sum << std::endl;

    return 0;
}
