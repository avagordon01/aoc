#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_line = *bp::upper >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(RRRRIICCFF
RRRRIICCCF
VVRRRCCFFF
VVRCCCJFFF
VVVVCJJCFE
VVIVCCJJEE
VVIIICJJEE
MIIIIIJJEE
MIIISIJEEE
MMMISSJEEE
)EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2024/12.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& grid = *result;

    using coord = Eigen::Vector2i;
    coord pos;
    const coord grid_size{grid[0].size(), grid.size()};
    const auto bounds_check = [grid_size](const auto& pos) {
        return md_bounds_check(grid_size, pos);
    };

    std::array<std::array<coord, 140>, 140> regions_grid;
    for (size_t i = 0; i < 140; i++) {
        for (size_t j = 0; j < 140; j++) {
            regions_grid[i][j] = coord{-1, -1};
        }
    }

    const auto dirs = dirs_2d_4;

    //label regions
    const auto flood_fill = [&](const auto start_pos) {
        const auto start_n = grid[start_pos[1]][start_pos[0]];
        std::deque<coord> to_visit;
        to_visit.push_back(start_pos);
        while (!to_visit.empty()) {
            const auto pos = to_visit.front();
            to_visit.pop_front();
            const auto& n = grid[pos[1]][pos[0]];
            if (n != start_n) {
                continue;
            }
            auto& r = regions_grid[pos[1]][pos[0]];
            if (r == coord{-1, -1}) {
                r = pos;
            }
            for (const auto& dir: dirs) {
                const auto new_pos = pos + dir;
                if (!bounds_check(new_pos)) {
                    continue;
                }
                const auto& new_n = grid[new_pos[1]][new_pos[0]];
                auto& new_r = regions_grid[new_pos[1]][new_pos[0]];
                if (new_r != coord{-1, -1}) {
                    continue;
                }
                if (new_n == start_n) {
                    new_r = r;
                }
                to_visit.push_back(new_pos);
            }
        }
    };
    for (pos[1] = 0; pos[1] < grid_size[1]; pos[1]++) {
        for (pos[0] = 0; pos[0] < grid_size[0]; pos[0]++) {
            flood_fill(pos);
        }
    }

    //calculate area
    std::unordered_map<coord, std::pair<size_t, size_t>> regions_area_perimeter;
    for (pos[1] = 0; pos[1] < grid_size[1]; pos[1]++) {
        for (pos[0] = 0; pos[0] < grid_size[0]; pos[0]++) {
            const auto& r = regions_grid[pos[1]][pos[0]];
            regions_area_perimeter[r].first++;
        }
    }

    //calculate perimeters
    for (pos[1] = 0; pos[1] < grid_size[1]; pos[1]++) {
        for (pos[0] = 0; pos[0] < grid_size[0]; pos[0]++) {
            const auto& r = regions_grid[pos[1]][pos[0]];
            for (const auto& dir: dirs) {
                const auto new_pos = pos + dir;
                if (!bounds_check(new_pos)) {
                    regions_area_perimeter[r].second++;
                    continue;
                }
                const auto& new_r = regions_grid[new_pos[1]][new_pos[0]];
                regions_area_perimeter[r].second += (new_r != r);
            }
        }
    }

    //total
    size_t total = 0;
    for (const auto& [r, area_perimeter]: regions_area_perimeter) {
        //std::println("r = {}, area = {}, perimeter = {}", r, area_perimeter.first, area_perimeter.second);
        const auto& n = grid[r[1]][r[0]];
        std::cout << "r = " << r << ", n = " << n << ", area = " << area_perimeter.first << ", perimeter = " << area_perimeter.second << std::endl;
        total += area_perimeter.first * area_perimeter.second;
    }

    std::cout << "part 1 = " << total << std::endl;

    //part 2
    uint64_t sum_p2 = 0;
    std::cout << "part 2 = " << sum_p2 << std::endl;

    return 0;
}
