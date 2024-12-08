#include "util.hh"

auto main() -> int {
    using namespace util;
    namespace bp = boost::parser;

    const auto p_line = bp::separate[bp::ulong_long >> ':' >> bp::blank >> bp::ulong_long % bp::blank >> bp::eol];
    const auto file = *p_line >> bp::eoi;

    const auto result = bp::parse(file_as_string("2024/7.input.txt"), file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    const auto& lines = *result;

    //part 1
    uint64_t sum = 0;
    for (const auto& line: lines) {
        const auto& [target, inputs] = line;
        size_t n = inputs.size() - 1;
        for (size_t i = 0; i < (1uz << n); i++) {
            uint64_t value = inputs.front();
            for (const auto& [index, input]: inputs | std::views::drop(1) | std::views::enumerate) {
                bool op = i & (1 << index);
                if (op) {
                    value += input;
                } else {
                    value *= input;
                }
            }
            if (value == target) {
                sum += target;
                break;
            }
        }
    }
    std::cout << "part 1 = " << sum << std::endl;

    //part 2
    const auto num_digits = [](const auto i) {
        return static_cast<int>(log10(i) + 1);
    };
    uint64_t sum_p2 = 0;
    for (const auto& line: lines) {
        const auto& [target, inputs] = line;
        size_t n = inputs.size() - 1;
        for (size_t i = 0; i < pow(3, n); i++) {
            uint64_t value = inputs.front();
            auto i_ = i;
            for (const auto& [index, input]: inputs | std::views::drop(1) | std::views::enumerate) {
                auto op = i_ % 3;
                i_ /= 3;
                if (op == 0) {
                    value += input;
                } else if (op == 1) {
                    value *= input;
                } else if (op == 2) {
                    value *= pow(10, num_digits(input));
                    value += input;
                }
            }
            if (value == target) {
                sum_p2 += target;
                break;
            }
        }
    }
    std::cout << "part 2 = " << sum_p2 << std::endl;

    return 0;
}
