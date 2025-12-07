#include "util.hh"

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line = *bp::char_(".^S") >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(.......S.......
...............
.......^.......
...............
......^.^......
...............
.....^.^.^.....
...............
....^.^...^....
...............
...^.^...^.^...
...............
..^...^.....^..
...............
.^.^.^.^.^...^.
...............
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
    std::set<size_t> positions;
    for (size_t i = 0; i < input[0].size(); i++) {
        const auto c = input[0][i];
        if (c == 'S') {
            positions.insert(i);
            break;
        }
    }
    size_t splits = 0;
    for (const auto& line: input) {
        std::set<size_t> next_positions;
        for (auto pos: positions) {
            const auto c = line[pos];
            if (c == '^') {
                splits += 1;
                next_positions.insert(pos - 1);
                next_positions.insert(pos + 1);
            } else {
                next_positions.insert(pos);
            }
        }
        positions = next_positions;
    }
    std::cout << "part 1 = " << splits << std::endl;

    //part 2
    size_t width = input[0].size();
    std::vector<size_t> path_counts(width, 0);
    for (size_t i = 0; i < width; i++) {
        const auto c = input[0][i];
        if (c == 'S') {
            path_counts[i] = 1;
            break;
        }
    }
    for (const auto& line: input) {
        std::vector<size_t> next_path_counts(width, 0);
        for (size_t i = 0; i < width; i++) {
            const auto path_count = path_counts[i];
            const auto c = line[i];
            if (c == '^') {
                next_path_counts[i - 1] += path_count;
                next_path_counts[i + 1] += path_count;
            } else {
                next_path_counts[i] += path_count;
            }
        }
        path_counts = next_path_counts;
    }
    size_t count = 0;
    for (const auto& c: path_counts) {
        count += c;
    }
    std::cout << "part 2 = " << count << std::endl;

    return 0;
}
