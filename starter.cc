#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_line = bp::ulong_long % bp::blank >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(
)EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2024/7.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& lines = *result;

    //part 1
    uint64_t total_p1 = 0;
    std::cout << "part 1 = " << total_p1 << std::endl;

    //part 2
    uint64_t total_p2 = 0;
    std::cout << "part 2 = " << total_p2 << std::endl;

    return 0;
}
