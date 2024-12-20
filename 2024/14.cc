#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_line =
        "p=" >> bp::int_ >> "," >> bp::int_ >> " " >>
        "v=" >> bp::int_ >> "," >> bp::int_ >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(
)EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2024/14.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    using coord = Eigen::Vector2i;

    auto robots = *result | std::views::transform([](const auto& t) {
        const auto& [a, b, c, d] = t;
        return std::make_pair(coord(a, b), coord(c, d));
    }) | std::ranges::to<std::vector>();
    const auto robots_copy = robots;

    //part 1
    const auto grid_size = coord(101, 103);
    for (size_t i = 0; i < 100; i++) {
        for (auto& [position, velocity]: robots) {
            position += velocity;
            position[0] = positive_mod(position[0], grid_size[0]);
            position[1] = positive_mod(position[1], grid_size[1]);
        }
    }
    const auto quadrants_min = std::array<coord, 4>{{
        {0, 0},
        {0, grid_size[1] / 2 + 1},
        {grid_size[0] / 2 + 1, 0},
        {grid_size[0] / 2 + 1, grid_size[1] / 2 + 1},
    }};
    auto quadrants_counts = std::array<size_t, 4>{};
    for (const auto& [position, _]: robots) {
        for (const auto& [i, min]: quadrants_min | std::views::enumerate) {
            const auto max = min + grid_size / 2;
            const auto inside = (min.array() <= position.array() && position.array() < max.array()).all();
            quadrants_counts[i] += inside;
        }
    }
    const size_t total_p1 = std::ranges::fold_left(quadrants_counts, 1, std::multiplies<size_t>());
    std::cout << "part 1 = " << total_p1 << std::endl;

    //part 2
    robots = robots_copy;
    const size_t n = grid_size[0] * grid_size[1];
    std::vector<size_t> connected_neighbour_counts(n);
    mdbitset<101, 103> grid;
    for (size_t i = 1; i < n; i++) {
        for (auto& [position, velocity]: robots) {
            position += velocity;
            position[0] = positive_mod(position[0], grid_size[0]);
            position[1] = positive_mod(position[1], grid_size[1]);
        }
        grid.reset();
        for (const auto& [position, _]: robots) {
            grid[position] = true;
        }
        coord pos;
        size_t connected_neighbour_count = 0;
        for (pos[1] = 0; pos[1] < grid_size[1]; pos[1]++) {
            for (pos[0] = 0; pos[0] < grid_size[0]; pos[0]++) {
                if (!grid[pos]) {
                    continue;
                }
                for (const auto& dir: dirs_2d_4) {
                    const coord new_pos = pos + dir;
                    if (!md_bounds_check(grid_size, new_pos)) {
                        continue;
                    }
                    const auto connected = grid[new_pos];
                    connected_neighbour_count += connected;
                }
            }
        }
        connected_neighbour_counts[i] = connected_neighbour_count;
        if (i == 6620) {
            std::cout << grid;
        }
    }
    size_t max_i = std::ranges::max_element(connected_neighbour_counts) - connected_neighbour_counts.begin();
    std::cout << "part 2 = " << max_i << std::endl;

    return 0;
}
