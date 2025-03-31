#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    {
        const auto p_junk = *bp::skip[bp::lower];
        const auto p_line = p_junk >> bp::digit % p_junk >> bp::eol;
        const auto p_file = *p_line >> bp::eoi;

        const auto result = bp::parse(file_as_string("2023/1.input.txt"), p_file);
        if (!result) {
            std::cerr << "failed to parse input" << std::endl;
            return 1;
        }

        const auto& lines = *result;

        for (const auto& line: lines) {
            /*
            const auto l = line.front();
            const auto r = line.back();
            const auto num = l * 10 + r;
            */
        }
    }

    {
        const bp::symbols<size_t> p_numbers = {
            {"one", 1},
            {"two", 2},
            {"three", 3},
            {"four", 4},
            {"five", 5},
            {"six", 6},
            {"seven", 7},
            {"eight", 8},
            {"nine", 9},
        };
        const auto p_num = bp::digit | p_numbers;
        const auto p_junk = bp::skip[*(bp::lower - p_num)];
        const auto p_line = p_junk >> p_num >> p_junk >> p_num >> p_junk;
        const auto p_file = *p_line;

        const auto result = bp::parse(file_as_string("2023/1.input.txt"), p_file);
        if (!result) {
            std::cerr << "failed to parse input" << std::endl;
            return 1;
        }

        const auto& lines = *result;

        /*
        int total = 0;
        for (const auto& line: lines) {
            const auto [a, b] = line;
            const auto num = a * 10 + b;
            total += num;
        }
        std::cout << "part 1 = " << total << std::endl;
        */
    }
    return 0;
}
