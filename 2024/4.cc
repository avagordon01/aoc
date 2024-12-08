#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    std::array chars = {'X', 'M', 'A', 'S'};
    const auto line = *bp::char_(chars) >> bp::eol;
    const auto file = *line >> bp::eoi;

    const auto result = bp::parse(file_as_string("2024/4.input.txt"), file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    std::array<std::pair<int, int>, 8> dirs = {{
        {1, 0},
        {1, 1},
        {0, 1},
        {-1, 1},
        {-1, 0},
        {-1, -1},
        {0, -1},
        {1, -1},
    }};

    const auto& grid = *result;

    size_t match_count = 0;
    size_t width = grid[0].size();
    size_t height = grid.size();
    for (size_t y0 = 0; y0 < height; y0++) {
        for (size_t x0 = 0; x0 < width; x0++) {
            if (grid[y0][x0] != chars[0]) {
                continue;
            }
            for (const auto& [dir_x, dir_y]: dirs) {
                size_t x = x0;
                size_t y = y0;
                size_t i;
                for (i = 1; i < chars.size(); i++) {
                    x += dir_x;
                    y += dir_y;
                    if (x >= width || y >= height) {
                        break;
                    }
                    if (grid[y][x] != chars[i]) {
                        break;
                    }
                }
                if (i == chars.size()) {
                    match_count++;
                }
            }
        }
    }
    std::cout << "part 1 = " << match_count << std::endl;

    size_t match_count_p2 = 0;
    for (size_t y0 = 1; y0 < height - 1; y0++) {
        for (size_t x0 = 1; x0 < width - 1; x0++) {
            if (grid[y0][x0] != 'A') {
                continue;
            }
            std::array x = {
                grid[y0 + 1][x0 + 1],
                grid[y0 + 1][x0 - 1],
                grid[y0 - 1][x0 - 1],
                grid[y0 - 1][x0 + 1],
            };
            if (
                x == std::array{'M', 'M', 'S', 'S'} ||
                x == std::array{'M', 'S', 'S', 'M'} ||
                x == std::array{'S', 'S', 'M', 'M'} ||
                x == std::array{'S', 'M', 'M', 'S'}
            ) {
                match_count_p2++;
            }
        }
    }
    std::cout << "part 2 = " << match_count_p2 << std::endl;

    return 0;
}
