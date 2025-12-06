#include "util.hh"

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line = *bp::char_(".@") >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(..@@.@@@@.
@@@.@.@.@@
@@@@@.@.@@
@.@@@@..@.
@@.@@@@.@@
.@@@@@@@.@
.@.@.@.@@@
@.@@@.@@@@
.@@@@@@@@.
@.@.@@@.@.
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
    size_t w = input[0].size();
    size_t h = input.size();
    const auto bounds_check = [](const auto bounds, const auto index){
        for (size_t i = 0; i < static_cast<size_t>(bounds.size()); i++) {
            if (index[i] >= 0 && index[i] < bounds[i]) {
            } else {
                return false;
            }
        }
        return true;
    };
    std::deque<Eigen::Vector2i> to_remove;
    const auto bounds = Eigen::Vector2i(w, h);
    size_t sum = 0;
    std::vector<size_t> counts(w * h, 0);
    for (size_t y = 0; y < h; y++) {
        for (size_t x = 0; x < w; x++) {
            const auto pos = Eigen::Vector2i(x, y);
            size_t neighbour_sum = 0;
            for (const auto dir: dirs_2d_8) {
                const auto new_pos = pos + dir;
                if (!bounds_check(bounds, new_pos)) {
                    continue;
                }
                const auto c = input[new_pos.y()][new_pos.x()];
                if (c == '@') {
                    neighbour_sum += 1;
                }
            }
            counts[y * w + x] = neighbour_sum;
            if (input[y][x] == '@' && neighbour_sum < 4) {
                sum += 1;
                to_remove.push_back({x, y});
            } else {
            }
        }
    }
    std::cout << "part 1 = " << sum << std::endl;

    //part 2
    sum = 0;
    while (!to_remove.empty()) {
        const auto pos = to_remove.front();
        to_remove.pop_front();
        for (const auto dir: dirs_2d_8) {
            const auto new_pos = pos + dir;
            if (!bounds_check(bounds, new_pos)) {
                continue;
            }
            const auto c = input[new_pos.y()][new_pos.x()];
            if (c != '@') {
                continue;
            }
            auto& count = counts[new_pos.y() * w + new_pos.x()];
            const auto old_count = count;
            if (count > 0) {
                count -= 1;
            }
            if (count < 4 && old_count >= 4) {
                to_remove.push_back(new_pos);
            }
        }
        sum += 1;
    }
    std::cout << "part 2 = " << sum << std::endl;

    return 0;
}
