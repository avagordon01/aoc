#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_line = bp::ulong_long >> ',' >> bp::ulong_long >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto result = bp::parse(file_as_string("2024/18.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& byte_coords = *result;

    using coord = Eigen::Vector2i;

    const auto grid_size = coord(71, 71);

    mdbitset<71, 71> blocked{};
    for (const auto& [x, y]: byte_coords | std::views::take(1024)) {
        blocked[x, y] = true;
    }

    const auto start = coord::Zero();
    const auto end = grid_size - coord::Ones();
    const auto dirs = dirs_2d_4;

    const auto heuristic = [&](const auto& a) {
        return (end - a).array().abs().sum();
    };

    //part 1
    const auto a_star_search = [&](const auto start, const auto end, const auto heuristic) -> std::optional<size_t> {
        const auto compare = [&](const auto& t_a, const auto& t_b) {
            const auto& [a, a_cost] = t_a;
            const auto& [b, b_cost] = t_b;
            return a_cost + heuristic(a) > b_cost + heuristic(b);
        };
        std::priority_queue<
            std::pair<coord, size_t>,
            std::vector<std::pair<coord, size_t>>,
            decltype(compare)
        > to_visit(compare);
        to_visit.push({start, 0});

        mdbitset<71, 71> visited{};

        while (!to_visit.empty()) {
            const auto [pos, cost] = to_visit.top();
            to_visit.pop();
            visited[pos] = true;
            if (pos == end) {
                return {cost};
            }
            for (const auto& dir: dirs) {
                const auto next_pos = pos + dir;
                if (md_bounds_check(grid_size, next_pos) &&
                    !visited[next_pos] &&
                    !blocked[next_pos]
                ) {
                    to_visit.push({next_pos, cost + 1});
                }
            }
        }
        return std::nullopt;
    };
    std::cout << "part 1 = " << a_star_search(start, end, heuristic).value() << std::endl;

    //part 2
    for (const auto& [x, y]: byte_coords | std::views::drop(1024)) {
        blocked[x, y] = true;

        if (!a_star_search(start, end, heuristic)) {
            std::cout << "part 2 = " << x << ',' << y << std::endl;
            break;
        }
    }

    return 0;
}
