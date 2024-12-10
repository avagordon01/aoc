#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_line = *bp::digit >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732
)EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2024/10.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& grid = *result;

    using coord = Eigen::Vector2i;
    const coord grid_size{grid[0].size(), grid.size()};
    const auto bounds_check = [grid_size](const auto& pos) {
        return md_bounds_check(grid_size, pos);
    };

    //part 1
    std::unordered_set<coord> zeros;
    for (coord pos{}; pos[1] < grid_size[1]; pos[1]++) {
        for (pos[0] = 0; pos[0] < grid_size[0]; pos[0]++) {
            const char& c = grid[pos[1]][pos[0]];
            if (c == '0') {
                zeros.insert(pos);
            }
        }
    }
    const auto dirs = std::array<coord, 4>{{
        {0, 1},
        {1, 0},
        {0, -1},
        {-1, 0},
    }};
    const auto traverse = [&](coord start_pos) -> size_t {
        size_t count = 0;
        std::vector<coord> next_pos{start_pos};
        std::unordered_set<coord> visited;
        while (!next_pos.empty()) {
            const auto pos = next_pos.back();
            next_pos.pop_back();
            visited.insert(pos);
            const auto& c = grid[pos[1]][pos[0]];
            if (c == '9') {
                count++;
            }
            for (const auto& dir: dirs) {
                const auto new_pos = pos + dir;
                if (!bounds_check(new_pos)) {
                    continue;
                }
                const auto& new_c = grid[new_pos[1]][new_pos[0]];
                if (new_c == c + 1 && !visited.contains(new_pos)) {
                    next_pos.push_back(new_pos);
                }
            }
        }
        return count;
    };
    size_t count = 0;
    for (const auto& pos: zeros) {
        count += traverse(pos);
    }
    std::cout << "part 1 = " << count << std::endl;

    //part 2
    const auto traverse_2 = [&](coord start_pos) -> size_t {
        size_t count = 0;
        std::vector<coord> next_pos{start_pos};
        while (!next_pos.empty()) {
            const auto pos = next_pos.back();
            next_pos.pop_back();
            const auto& c = grid[pos[1]][pos[0]];
            if (c == '9') {
                count++;
            }
            for (const auto& dir: dirs) {
                const auto new_pos = pos + dir;
                if (!bounds_check(new_pos)) {
                    continue;
                }
                const auto& new_c = grid[new_pos[1]][new_pos[0]];
                if (new_c == c + 1) {
                    next_pos.push_back(new_pos);
                }
            }
        }
        return count;
    };
    count = 0;
    for (const auto& pos: zeros) {
        count += traverse_2(pos);
    }
    std::cout << "part 2 = " << count << std::endl;

    return 0;
}
