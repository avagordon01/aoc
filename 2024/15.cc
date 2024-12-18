#include "util.hh"

using coord = Eigen::Vector2i;

bool check_push(const auto& grid, const auto grid_size, auto pos, auto dir) {
    pos += dir;
    if (!md_bounds_check(grid_size, pos)) {
        return false;
    }
    const char& c = grid[pos[1]][pos[0]];
    if (c == '#') {
        return false;
    } else if (c == '.') {
        return true;
    } else if (dir[1] != 0 && c == ']') {
        return check_push(grid, grid_size, pos, dir) &&
            check_push(grid, grid_size, coord{pos[0] - 1, pos[1]}, dir);
    } else if (dir[1] != 0 && c == '[') {
        return check_push(grid, grid_size, pos, dir) &&
            check_push(grid, grid_size, coord{pos[0] + 1, pos[1]}, dir);
    } else if (dir[1] == 0 && c == ']') {
        return check_push(grid, grid_size, pos, dir);
    } else if (dir[1] == 0 && c == '[') {
        return check_push(grid, grid_size, pos, dir);
    } else {
        ASSERT(false, c);
        return false;
    }
}

void do_push(auto& grid, const auto grid_size, auto pos, auto dir) {
    const auto old_pos = pos;
    pos += dir;
    const char& c = grid[pos[1]][pos[0]];
    if (c == '.') {
        grid[pos[1]][pos[0]] = grid[old_pos[1]][old_pos[0]];
        grid[old_pos[1]][old_pos[0]] = '.';
    } else if (dir[1] != 0 && c == ']') {
        do_push(grid, grid_size, pos, dir);
        do_push(grid, grid_size, coord{pos[0] - 1, pos[1]}, dir);
    } else if (dir[1] != 0 && c == '[') {
        do_push(grid, grid_size, pos, dir);
        do_push(grid, grid_size, coord{pos[0] + 1, pos[1]}, dir);
    } else if (dir[1] == 0 && c == ']') {
        do_push(grid, grid_size, pos, dir);
    } else if (dir[1] == 0 && c == '[') {
        do_push(grid, grid_size, pos, dir);
    } else {
        ASSERT(false, c);
    }
}

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_cell = bp::char_("#.O@");
    const auto p_grid_line = +p_cell >> bp::eol;
    const auto p_move = bp::char_("><v^");
    const auto p_file = bp::separate[*p_grid_line >> bp::skip(bp::eol)[*p_move]] >> bp::eol >> bp::eoi;

    const auto example = std::string{R"EOF(########
#..O.O.#
##@.O..#
#...O..#
#.#.O..#
#...O..#
#......#
########

<^^>>>vv<v>>v<<
)EOF"};

    const auto example_2 = std::string{R"EOF(##########
#..O..O.O#
#......O.#
#.OO..O.O#
#..O@..O.#
#O#..O...#
#O..O..O.#
#.OO.O.OO#
#....O...#
##########

<vv>^<v^>v>^vv^v>v<>v^v<v<^vv<<<^><<><>>v<vvv<>^v^>^<<<><<v<<<v^vv^v>^
vvv<<^>^v^^><<>>><>^<<><^vv^^<>vvv<>><^^v>^>vv<>v<<<<v<^v>^<^^>>>^<v<v
><>vv>v^v^<>><>>>><^^>vv>v<^^^>>v^v^<^^>v^^>v^<^v>v<>>v^v^<v>v^^<^^vv<
<<v<^>>^^^^>>>v^<>vvv^><v<<<>^^^vv^<vvv>^>v<^^^^v<>^>vvvv><>>v^<<^^^^^
^><^><>>><>^^<<^^v>>><^<v>^<vv>>v>>>^v><>^v><<<<v>>v<v<v>vvv>^<><<>^><
^>><>^v<><^vvv<^^<><v<<<<<><^v<<<><<<^^<v<^^^><^>>^<v^><<<^>>^v<v^v<v^
>^>>^v>vv>^<<^v<>><<><<v<<v><>v<^vv<<<>^^v^>^^>>><<^v>>v^v><^^>>^<>vv^
<><^^>^^^<><vvvvv^v<v<<>^v<v>v<<^><<><<><<<^^<<<^<<>><<><^^^>^^<>^>v<>
^^>vv<^v^v<vv>^<><v<^v>^^^>>>^^vvv^>vvv<>>>^<^>>>>>^<<^v>^vvv<>^<><<v>
v^^>>><<^^<>>^v^<v^vv<>v^<<>^<^v^v><^<<<><<^<v><v<>vv>>v><v^<vv<>v^<<^
)EOF"};

    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(example_2, p_file);
    //const auto result = bp::parse(file_as_string("2024/15.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& [grid_orig, moves] = *result;

    auto grid = grid_orig;
    const auto grid_size = coord{grid[0].size(), grid.size()};

    //find initial robot position
    coord pos;
    auto initial_robot_position = coord(0, 0);
    for (pos[1] = 0; pos[1] < grid_size[1]; pos[1]++) {
        bool found = false;
        for (pos[0] = 0; pos[0] < grid_size[0]; pos[0]++) {
            char& c = grid[pos[1]][pos[0]];
            if (c == '@') {
                c = '.';
                initial_robot_position = pos;
                found = true;
                break;
            }
        }
        if (found) {
            break;
        }
    }

    //apply moves
    const auto move_dirs = std::unordered_map<char, coord>{
        {'^', coord{0, -1}},
        {'>', coord{1, 0}},
        {'<', coord{-1, 0}},
        {'v', coord{0, 1}},
    };
    const auto find_first_empty = [&](auto pos, auto dir) -> std::optional<coord> {
        while (true) {
            pos += dir;
            if (!md_bounds_check(grid_size, pos)) {
                return std::nullopt;
            }
            const char& c = grid[pos[1]][pos[0]];
            if (c == '#') {
                return std::nullopt;
            }
            if (c == '.') {
                return {pos};
            }
            ASSERT(c == 'O');
        }
    };
    auto robot_pos = initial_robot_position;
    for (const auto& move: moves) {
        const auto dir = move_dirs.at(move);
        const auto maybe_empty_pos = find_first_empty(robot_pos, dir);
        if (!maybe_empty_pos) {
            continue;
        }
        robot_pos += dir;
        const auto empty_pos = *maybe_empty_pos;
        ASSERT(empty_pos[0] == robot_pos[0] || empty_pos[1] == robot_pos[1]);
        std::swap(grid[empty_pos[1]][empty_pos[0]], grid[robot_pos[1]][robot_pos[0]]);
        char& c = grid[robot_pos[1]][robot_pos[0]];
    }

    //calculate checksum
    uint64_t checksum = 0;
    for (pos[1] = 0; pos[1] < grid_size[1]; pos[1]++) {
        for (pos[0] = 0; pos[0] < grid_size[0]; pos[0]++) {
            char& c = grid[pos[1]][pos[0]];
            if (c == 'O') {
                checksum += 100 * static_cast<uint64_t>(pos[1]) + static_cast<uint64_t>(pos[0]);
            }
        }
    }
    std::cout << "part 1 = " << checksum << std::endl;

    //part 2

    //generate wide box map
    {
    std::vector<std::vector<char>> wide_grid;
    for (const auto& line: grid_orig) {
        wide_grid.push_back({});
        for (const auto& c: line) {
            if (c == '.') {
                wide_grid.back().push_back('.');
                wide_grid.back().push_back('.');
            } else if (c == '#') {
                wide_grid.back().push_back('#');
                wide_grid.back().push_back('#');
            } else if (c == 'O') {
                wide_grid.back().push_back('[');
                wide_grid.back().push_back(']');
            } else if (c == '@') {
                wide_grid.back().push_back('@');
                wide_grid.back().push_back('.');
            } else {
                ASSERT(false, c);
            }
        }
    }
    std::cout << wide_grid << std::endl;

    const auto wide_grid_size = coord{wide_grid[0].size(), wide_grid.size()};

    auto initial_robot_position = coord{0, 0};
    for (pos[1] = 0; pos[1] < wide_grid_size[1]; pos[1]++) {
        bool found = false;
        for (pos[0] = 0; pos[0] < wide_grid_size[0]; pos[0]++) {
            char& c = wide_grid[pos[1]][pos[0]];
            if (c == '@') {
                c = '.';
                found = true;
                initial_robot_position = pos;
                break;
            }
        }
        if (found) {
            break;
        }
    }

    //
    auto robot_pos = initial_robot_position;
    std::cout << robot_pos << std::endl;
    for (const auto& move: moves) {
        const auto dir = move_dirs.at(move);
        if (!check_push(wide_grid, wide_grid_size, robot_pos, dir)) {
            continue;
        }
        do_push(wide_grid, wide_grid_size, robot_pos, dir);
        robot_pos += dir;

        wide_grid[robot_pos[1]][robot_pos[0]] = '@';
        std::cout << wide_grid << std::endl;
        wide_grid[robot_pos[1]][robot_pos[0]] = '.';
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
    std::cout << wide_grid << std::endl;

    //calculate checksum
    uint64_t checksum = 0;
    for (pos[1] = 0; pos[1] < wide_grid_size[1]; pos[1]++) {
        for (pos[0] = 0; pos[0] < wide_grid_size[0]; pos[0]++) {
            char& c = wide_grid[pos[1]][pos[0]];
            if (c == '[') {
                checksum += 100 * static_cast<uint64_t>(pos[1]) + static_cast<uint64_t>(pos[0]);
            }
        }
    }
    std::cout << "part 2 = " << checksum << std::endl;
    }

    return 0;
}
