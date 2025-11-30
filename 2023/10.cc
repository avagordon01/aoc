#include "util.hh"
#include <boost/parser/parser.hpp>

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line = *bp::char_("|-LJ7F.S") >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF()EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string(binary_name_to_test_file_name(argv[0])), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& input = *result;

    //part 1
    std::pair<size_t, size_t> start_pos;
    for (size_t y = 0; y < input.size(); y++) {
        for (size_t x = 0; x < input[y].size(); x++) {
            if (input[y][x] == 'S') {
                start_pos = {x, y};
            }
        }
    }
    std::unordered_map<char, std::vector<std::pair<int, int>>> char_to_dirs = {
        {'|', {
            {0, 1},
            {0, -1},
        }},
        {'-', {
            {-1, 0},
            {1, 0},
        }},
        {'L', {
            {0, -1},
            {1, 0},
        }},
        {'J', {
            {0, -1},
            {-1, 0},
        }},
        {'7', {
            {0, 1},
            {-1, 0},
        }},
        {'F', {
            {0, 1},
            {1, 0},
        }},
        {'.', {
        }},
        {'S', {
            {0, 1},
            {0, -1},
        }},
    };
    std::unordered_map<std::pair<size_t, size_t>, size_t> visited;
    visited[start_pos] = 0;
    std::deque<std::pair<size_t, size_t>> to_visit;
    to_visit.push_back(start_pos);
    size_t last_count = 0;
    while (!to_visit.empty()) {
        const auto pos = to_visit.front();
        to_visit.pop_front();
        const auto count = visited[pos];
        const auto c = input[pos.second][pos.first];
        const auto& next_dirs = char_to_dirs[c];
        for (const auto& dir: next_dirs) {
            auto next_pos = pos;
            next_pos.first += dir.first;
            next_pos.second += dir.second;
            if (next_pos.first >= input[0].size()) {
                continue;
            }
            if (next_pos.second >= input.size()) {
                continue;
            }
            if (visited.contains(next_pos)) {
                continue;
            }
            visited[next_pos] = count + 1;
            last_count = count + 1;
            to_visit.push_back(next_pos);
        }
    }
    std::cout << "part 1 = " << last_count << std::endl;

    //part 2
    // imagine a point in the top left corner of the grid cell
    // if moving to the next (x+1) grid cell top left corner would cross a pipe
    // then the toggle is true
    std::unordered_map<char, bool> char_to_winding_order_toggle = {
        {'|', true},
        {'-', false},
        {'L', true},
        {'J', true},
        {'7', false},
        {'F', false},
        {'.', false},
        {'S', true},
    };
    size_t count_inside = 0;
    for (size_t y = 0; y < input.size(); y++) {
        bool inside = false;
        for (size_t x = 0; x < input[0].size(); x++) {
            const auto pos = std::make_pair(x, y);
            const auto on_main_pipe = visited.contains(pos);
            if (!on_main_pipe) {
                count_inside += inside;
                continue;
            } else {
                const auto c = input[y][x];
                const auto toggle = char_to_winding_order_toggle[c];
                if (toggle) {
                    inside = !inside;
                }
            }
        }
    }
    std::cout << "part 2 = " << count_inside << std::endl;

    return 0;
}
