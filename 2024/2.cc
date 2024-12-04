#include <boost/parser/parser.hpp>
#include <boost/hana.hpp>
#include <vector>
#include <unordered_map>
#include <cmath>
#include "util.hh"

auto main() -> int {
    //parse input
    namespace bp = boost::parser;
    using namespace boost::parser::literals;

    auto parser = ((bp::long_long % ' ') % '\n') >> bp::ws;

    auto result = bp::parse(file_as_string("2.input.txt"), parser);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    //part 1
    const auto& reports = *result;

    const auto safe_check = [](int s, int a, int b) {
        return s == sign(b - a) &&
            std::abs(b - a) >= 1 &&
            std::abs(b - a) <= 3;
    };

    size_t num_safe = 0;
    for (const auto& report: reports) {
        int s = sign(report[1] - report[0]);
        bool all_safe = true;
        for (size_t i = 0; i < report.size() - 1; i++) {
            all_safe &= safe_check(s, report[i], report[i + 1]);
        }

        num_safe += all_safe;
    }
    std::cout << "part 1 = " << num_safe << std::endl;

    //part 2
    size_t num_safe_2 = 0;
    for (const auto& report: reports) {
        if (report.size() <= 2) {
            num_safe_2 += 1;
            continue;
        }
        auto s_total = 0;
        for (size_t i = 0; i < report.size() - 1; i++) {
            const auto s = sign(report[i + 1] - report[i]);
            s_total += s;
        }
        const auto s = sign(s_total);

        bool all_safe = true;
        bool used_skip = false;
        size_t error_count = 0;
        for (size_t i = 0; i < report.size() - 1; i++) {
            if (safe_check(s, report[i], report[i + 1])) {
            } else if (!used_skip) {
                //FIXME no way to skip the first item
                if (i + 2 < report.size() && safe_check(s, report[i], report[i + 2])) {
                    used_skip = true;
                    i++;
                } else {
                    all_safe = false;
                    break;
                }
            } else {
                all_safe = false;
                break;
            }
        }

        num_safe_2 += all_safe;
    }
    std::cout << "part 2 = " << num_safe_2 << std::endl;

    return 0;
}
