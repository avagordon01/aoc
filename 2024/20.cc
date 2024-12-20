#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_line = +bp::char_(".#SE") >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(###############
#...#...#.....#
#.#.#.#.#.###.#
#S#...#.#.#...#
#######.#.#.###
#######.#.#...#
#######.#.###.#
###..E#...#...#
###.#######.###
#...###...#...#
#.#####.#.###.#
#.#...#.#.#...#
#.#.#.#.#.#.###
#...#...#...###
###############
)EOF"};
    //auto result = bp::parse(example, p_file);
    auto result = bp::parse(file_as_string("2024/20.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    auto& grid = *result;

    using coord = Eigen::Vector2i;
    coord pos;
    const auto grid_size = coord(grid[0].size(), grid.size());

    //part 1
    auto start_pos = coord(0, 0);
    auto end_pos = coord(0, 0);
    for (pos[1] = 0; pos[1] < grid_size[1]; pos[1]++) {
        for (pos[0] = 0; pos[0] < grid_size[0]; pos[0]++) {
            char& c = grid[pos[1]][pos[0]];
            if (c == 'S') {
                c = '.';
                start_pos = pos;
            }
            if (c == 'E') {
                c = '.';
                end_pos = pos;
            }
        }
    }
    const auto dirs = dirs_2d_4;
    std::array<std::array<uint64_t, 141>, 141> distance_from_start{};
    std::array<std::array<uint64_t, 141>, 141> distance_from_end{};
    uint64_t normal_best_time = 0;
    const auto fill_distance = [&](const auto start_pos, const auto end_pos, auto& grid_to_fill) {
        coord pos = start_pos;
        mdbitset<141, 141> visited{false};
        uint64_t i;
        for (i = 0; pos != end_pos; i++) {
            visited[pos] = true;
            grid_to_fill[pos[1]][pos[0]] = i;
            normal_best_time = i;
            for (const auto& dir: dirs) {
                const auto next_pos = pos + dir;
                const auto& c = grid[next_pos[1]][next_pos[0]];
                if (!md_bounds_check(grid_size, next_pos)) {
                    continue;
                }
                if (c != '.') {
                    continue;
                }
                if (visited[next_pos]) {
                    continue;
                }
                pos = next_pos;
                break;
            }
        }
        normal_best_time++;
        grid_to_fill[end_pos[1]][end_pos[0]] = normal_best_time;
    };
    fill_distance(start_pos, end_pos, distance_from_start);
    fill_distance(end_pos, start_pos, distance_from_end);
    uint64_t threshold_time = normal_best_time - 100;

    uint64_t cheat_count = 0;
    for (pos[1] = 0; pos[1] < grid_size[1]; pos[1]++) {
        for (pos[0] = 0; pos[0] < grid_size[0]; pos[0]++) {
            char& c = grid[pos[1]][pos[0]];
            if (c != '.') {
                continue;
            }
            for (const auto& dir: dirs) {
                const auto cheat_pos = pos + 2 * dir;
                if (!md_bounds_check(grid_size, cheat_pos)) {
                    continue;
                }
                const auto cheat_c = grid[cheat_pos[1]][cheat_pos[0]];
                if (cheat_c != '.') {
                    continue;
                }
                uint64_t new_time = distance_from_start[pos[1]][pos[0]] + 2 +
                    distance_from_end[cheat_pos[1]][cheat_pos[0]];
                if (new_time <= threshold_time) {
                    cheat_count++;
                }
            }
        }
    }
    std::cout << "part 1 = " << cheat_count << std::endl;

    //part 2
    const auto taxicab = [](const auto& a, const auto& b) -> auto {
        return (b - a).array().abs().sum();
    };

    {
        uint64_t cheat_count = 0;
        for (pos[1] = 0; pos[1] < grid_size[1]; pos[1]++) {
            for (pos[0] = 0; pos[0] < grid_size[0]; pos[0]++) {
                char& c = grid[pos[1]][pos[0]];
                if (c != '.') {
                    continue;
                }
                coord cheat_pos;
                int max_cheat_dist = 20;
                for (cheat_pos[1] = std::max(0, pos[1] - max_cheat_dist);
                    cheat_pos[1] <= std::min(grid_size[1] - 1, pos[1] + max_cheat_dist);
                    cheat_pos[1]++
                ) {
                    for (cheat_pos[0] = std::max(0, pos[0] - max_cheat_dist);
                        cheat_pos[0] <= std::min(grid_size[0] - 1, pos[0] + max_cheat_dist);
                        cheat_pos[0]++
                    ) {
                        const auto cheat_dist = taxicab(pos, cheat_pos);
                        if (cheat_dist > max_cheat_dist) {
                            continue;
                        }
                        const auto cheat_c = grid[cheat_pos[1]][cheat_pos[0]];
                        if (cheat_c != '.') {
                            continue;
                        }
                        uint64_t new_time = distance_from_start[pos[1]][pos[0]] + cheat_dist +
                            distance_from_end[cheat_pos[1]][cheat_pos[0]];
                        if (new_time <= threshold_time) {
                            cheat_count++;
                        }
                    }
                }
            }
        }
        std::cout << "part 2 = " << cheat_count << std::endl;
    }

    return 0;
}
