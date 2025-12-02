#include "util.hh"

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line = bp::char_("LR") >> bp::ulong_long >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(L68
L30
R48
L5
R60
L55
L1
L99
R14
L82
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
    int pos = 50;
    size_t zero_count = 0;
    for (const auto& [dir, count]: input) {
        if (dir == 'R') {
            pos += count;
        } else {
            pos -= count;
        }
        std::cout << pos << ", ";
        const auto before_pos = pos;
        pos = positive_mod(pos, 100);
        std::cout << pos << std::endl;
        if (pos == 0) {
            zero_count++;
        }
    }
    std::cout << "part 1 = " << zero_count << std::endl;

    //part 2
    pos = 50;
    zero_count = 0;
    for (const auto& [dir, count]: input) {
        int inc = 0;
        if (dir == 'R') {
            inc = 1;
        } else {
            inc = -1;
        }
        for (size_t i = 0; i < count; i++) {
            pos += inc;
            pos = positive_mod(pos, 100);
            if (pos == 0) {
                zero_count++;
            }
        }
    }
    std::cout << "part 2 = " << zero_count << std::endl;

    return 0;
}
