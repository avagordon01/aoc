#include "util.hh"
#include <algorithm>
#include <boost/parser/parser.hpp>

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line = (bp::ulong_long >> '-' >> bp::ulong_long) % ',' >> bp::eol;
    const auto p_file = p_line >> bp::eoi;

    const auto example = std::string{R"EOF(11-22,95-115,998-1012,1188511880-1188511890,222220-222224,1698522-1698528,446443-446449,38593856-38593862,565653-565659,824824821-824824827,2121212118-2121212124
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
    auto input_mut = input;
    std::ranges::sort(input_mut, std::less<size_t>(), [](auto p){ auto [a, b] = p; return a; });
    const auto is_invalid = [](size_t x) {
        const auto str = std::to_string(x);
        size_t digits = str.size();
        bool even = (digits % 2) == 0;
        if (!even) {
            return false;
        }
        digits /= 2;
        for (size_t i = 0; i < digits; i++) {
            if (str[i] != str[i + digits]) {
                return false;
            }
        }
        return true;
    };
    const auto next_invalid = [](size_t x) {
        size_t digits = std::to_string(x).size();
        bool even = (digits % 2) == 0;
        if (even) {
            digits /= 2;
            size_t mul = std::pow(10, digits);
            size_t l = x / mul;
            size_t r = x % mul;
            if (l > r) {
                x = l * mul + l;
            } else {
                l += 1;
                x = l * mul + l;
            }
        } else {
            digits /= 2;
            x = std::pow(10, digits);
            digits += 1;
            size_t mul = std::pow(10, digits);
            x = x * mul + x;
        }
        return x;
    };
    size_t sum = 0;
    for (const auto& [a, b]: input_mut) {
        size_t x = a;
        while (x <= b) {
            if (is_invalid(x)) {
                sum += x;
            }
            x += 1;
            x = next_invalid(x);
        }
    }
    std::cout << "part 1 = " << sum << std::endl;

    //part 2
    std::vector<size_t> all_invalid_ids;
    for (size_t i = 1; i < 100000; i++) {
        size_t digits = std::to_string(i).size();
        size_t mul = std::pow(10, digits);
        size_t val = i;
        for (size_t d = 2; d <= 10; d++) {
            val *= mul;
            val += i;
            if (val < 10000000000) {
                all_invalid_ids.push_back(val);
            }
        }
    }
    std::ranges::sort(all_invalid_ids);
    const auto ret = std::ranges::unique(all_invalid_ids);
    all_invalid_ids.erase(ret.begin(), ret.end());
    auto it = all_invalid_ids.begin();
    sum = 0;
    for (const auto& [a, b]: input_mut) {
        auto it_0 = std::find_if(it, all_invalid_ids.end(),
            [a](auto x){ return x >= a; }
        );
        const auto it_1 = std::find_if(it, all_invalid_ids.end(),
            [b](auto x){ return x > b; }
        );
        for (; it_0 != it_1; it_0++) {
            sum += *it_0;
        }
        it = it_1;
    }
    std::cout << "part 2 = " << sum << std::endl;

    return 0;
}
