#include "util.hh"

#include <boost/container_hash/hash.hpp>
#include <boost/container_hash/is_tuple_like.hpp>

//implements std::hash for pair, tuple, array, etc
namespace std {
    template <typename T>
    requires boost::container_hash::is_tuple_like<T>::value
    struct hash<T>
    {
    size_t operator()(const T& x) const
    {
        return boost::hash_value(x);
    }
    };
}

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_cell = bp::char_("#.SE");
    const auto p_grid_line = +p_cell >> bp::eol;
    const auto p_file = *p_grid_line >> bp::eoi;

    const auto example = std::string{R"EOF(###############
#.......#....E#
#.#.###.#.###.#
#.....#.#...#.#
#.###.#####.#.#
#.#.#.......#.#
#.#.#####.###.#
#...........#.#
###.#.#####.#.#
#...#.....#.#.#
#.#.#.###.#.#.#
#.....#...#.#.#
#.###.#.#.#.#.#
#S..#.....#...#
###############
)EOF"};
    auto result = bp::parse(example, p_file);
    //auto result = bp::parse(file_as_string("2024/16.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    auto& grid = *result;

    using coord = Eigen::Vector2i;
    const auto grid_size = coord(grid[0].size(), grid.size());


    coord end_pos = coord{0, 0};
    for (end_pos[1] = 0; end_pos[1] < grid_size[1]; end_pos[1]++) {
        bool found = false;
        for (end_pos[0] = 0; end_pos[0] < grid_size[0]; end_pos[0]++) {
            char& c = grid[end_pos[1]][end_pos[0]];
            if (c == 'E') {
                c = '.';
                found = true;
                break;
            }
        }
        if (found) {
            break;
        }
    }

    coord start_pos;
    for (start_pos[1] = 0; start_pos[1] < grid_size[1]; start_pos[1]++) {
        bool found = false;
        for (start_pos[0] = 0; start_pos[0] < grid_size[0]; start_pos[0]++) {
            char& c = grid[start_pos[1]][start_pos[0]];
            if (c == 'S') {
                c = '.';
                found = true;
                break;
            }
        }
        if (found) {
            break;
        }
    }

    const auto dirs = dirs_2d_4;

    const auto start_dir_i = 1uz;

    std::unordered_map<std::tuple<coord, size_t>, size_t> visited_scores;
    using T = std::tuple<coord, size_t, size_t>;
    const auto score_position = [&](const auto& t) {
        const auto& [pos, _, score] = t;
        return score;
        //return (end_pos - pos).array().abs().sum() * score;
    };
    const auto compare_scores = [&](const auto& t_a, const auto& t_b) {
        return score_position(t_a) > score_position(t_b);
    };
    std::priority_queue<T, std::vector<T>, decltype(compare_scores)> to_visit(compare_scores);

    to_visit.push({start_pos, start_dir_i, 0});

    while (!to_visit.empty()) {
        const auto [pos, dir_i, score] = to_visit.top();
        to_visit.pop();
        visited_scores.insert({{pos, dir_i}, score});

        if (pos == end_pos) {
            continue;
        }

        for (const auto dir_c: {-1, 0, +1}) {
            const auto dir_n = positive_mod(dir_i + dir_c, dirs.size());
            const auto dir = ((dir_c == 0) ? dirs[dir_n] : coord::Zero());
            const auto next_pos = pos + dir;
            if (grid[next_pos[1]][next_pos[0]] == '#') {
                continue;
            }
            const auto hypothetical_next_next_pos = pos + dirs[dir_n];
            if (grid[hypothetical_next_next_pos[1]][hypothetical_next_next_pos[0]] == '#') {
                continue;
            }
            const auto next_score = score + ((dir_c == 0) ? 1 : 1000);
            if (!md_bounds_check(grid_size, next_pos)) {
                continue;
            }
            auto it = visited_scores.find({next_pos, dir_n});
            if (it == visited_scores.end() || next_score < it->second) {
                to_visit.push({next_pos, dir_n, next_score});
            }
        }
    }

    size_t min_score = -1;
    std::vector<std::pair<coord, size_t>> best_paths_ends;
    for (const auto& [k, score]: visited_scores) {
        const auto& [pos, dir_i] = k;
        if (pos == end_pos) {
            min_score = std::min(score, min_score);
            best_paths_ends.push_back({pos, dir_i});
        }
    }

    std::cout << "part 1 = " << min_score << std::endl;

    //part 2
    std::unordered_set<coord> best_path_cells;
    std::cout << "best_paths_ends = " << best_paths_ends << std::endl;
    for (auto [pos, dir_i]: best_paths_ends) {
        while (pos != start_pos) {
            best_path_cells.insert(pos);
            const auto dir = dirs[dir_i];
            const auto score = visited_scores.at({pos, dir_i});
            bool match = false;
            for (const auto [prev_dir_i, prev_dir]: dirs | std::views::enumerate) {
                const auto d = (prev_dir == dir) ? dir : coord::Zero();
                const auto prev_pos = pos - d;
                const auto prev_score = score - ((prev_dir == dir) ? 1 : 1000);
                auto it = visited_scores.find({prev_pos, prev_dir_i});
                if (it != visited_scores.end() && it->second == prev_score) {
                    match = true;
                    std::tie(pos, dir_i) = it->first;
                    break;
                }
            }
            ASSERT(match, pos, dir_i);
            char& c = grid[pos[1]][pos[0]];
            c = 'O';
        }
        best_path_cells.insert(pos);
    }
    std::cout << grid << std::endl;
    std::cout << "part 2 = " << best_path_cells.size() << std::endl;

    return 0;
}
