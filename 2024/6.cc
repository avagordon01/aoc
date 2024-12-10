#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    //TODO
    //constexpr auto max_side_length = 130;
    //bp::transform(to_array<max_side_length>)[*p_cell]
    const auto p_cell = bp::char_(".#^");
    const auto p_row = *p_cell >> bp::eol;
    const auto file = *p_row >> bp::eoi;

    std::string example = R"EOF(....#.....
.........#
..........
..#.......
.......#..
..........
.#..^.....
........#.
#.........
......#...
)EOF";
    //const auto result = bp::parse(example, file);
    const auto result = bp::parse(file_as_string("2024/6.input.txt"), file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    std::vector<std::string> grid = *result;

    using namespace util;

    //part 1
    using coord = Eigen::Vector2i;
    coord pos;
    const coord grid_size{grid[0].size(), grid.size()};
    const auto bounds_check = [grid_size](const auto& pos) {
        return md_bounds_check(grid_size, pos);
    };
    bool found = false;
    for (pos[1] = 0; pos[1] < grid_size[1]; pos[1]++) {
        for (pos[0] = 0; pos[0] < grid_size[0]; pos[0]++) {
            char& c = grid[pos[1]][pos[0]];
            if (c == '^') {
                c = ' ';
                found = true;
                break;
            }
        }
        if (found) {
            break;
        }
    }
    const auto guard_pos = pos;

    const auto dirs = std::array<coord, 4>{{
        {0, -1},
        {1, 0},
        {0, 1},
        {-1, 0},
    }};
    int dir_index = 0;
    const auto guard_dir = dirs[dir_index];
    while (true) {
        if (!bounds_check(pos)) {
            break;
        }
        char& c = grid[pos[1]][pos[0]];
        if (c == '#') {
            const auto obstruction = pos;
            pos -= dirs[dir_index];
            dir_index = (dir_index + 1) % dirs.size();
        } else {
            c = ' ';
            pos += dirs[dir_index];
        }
    }

    size_t count = 0;
    for (pos[1] = 0; pos[1] < grid_size[1]; pos[1]++) {
        for (pos[0] = 0; pos[0] < grid_size[0]; pos[0]++) {
            char& c = grid[pos[1]][pos[0]];
            if (c == ' ') {
                c = '.';
                count++;
            }
        }
    }

    std::cout << "part 1 = " << count << std::endl;

    //part 2
    const auto inner_traverse = [&dirs, &bounds_check](auto& grid_copy, const coord start_pos, const size_t start_dir_index) {
        auto pos = start_pos;
        auto dir_index = start_dir_index;
        while (true) {
            if (!bounds_check(pos)) {
                //no loop
                return false;
            }
            char& c = grid_copy[pos[1]][pos[0]];
            if (c == '#') {
                //obstructed
                pos -= dirs[dir_index];
                dir_index = (dir_index + 1) % dirs.size();
            } else if (c == '.') {
                //unvisited
                c = (1u << dir_index);
                pos += dirs[dir_index];
            } else if (c & (1u << dir_index)) {
                //already visited with this direction
                return true;
            } else {
                //visited in a different direction
                c |= (1u << dir_index);
                pos += dirs[dir_index];
            }
        }
    };
    {
        auto pos = guard_pos;
        auto dir_index = 0;
        std::unordered_set<coord> obstacles;
        while (true) {
            if (!bounds_check(pos)) {
                break;
            }
            auto grid_copy = grid;
            auto new_obstacle_pos = pos + dirs[dir_index];
            bool skip = false;
            {
                //insert an obstacle
                //if it would be outside the grid, or there is already an obstacle there, skip
                if (!bounds_check(new_obstacle_pos)) {
                    skip = true;
                }
                if (!skip) {
                    char& c = grid_copy[new_obstacle_pos[1]][new_obstacle_pos[0]];
                    if (c == '#') {
                        skip = true;
                    }
                    c = '#';
                }
            }
            if (!skip) {
                if (inner_traverse(grid_copy, pos, dir_index)) {
                    obstacles.insert(new_obstacle_pos);
                }
            }
            char& c = grid[pos[1]][pos[0]];
            /*
            if (c == '#') {
                pos -= dirs[dir_index];
                dir_index = (dir_index + 1) % dirs.size();
            } else {
                pos += dirs[dir_index];
            }
            */
            if (c == '#') {
                //obstructed
                pos -= dirs[dir_index];
                dir_index = (dir_index + 1) % dirs.size();
            } else if (c == '.') {
                //unvisited
                c = (1u << dir_index);
                pos += dirs[dir_index];
            } else if (c & (1u << dir_index)) {
                //already visited with this direction
                return true;
            } else {
                //visited in a different direction
                c |= (1u << dir_index);
                pos += dirs[dir_index];
            }
        }
        std::cout << "part 2 = " << obstacles.size() << std::endl;
    }

    return 0;
}
