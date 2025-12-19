#include "util.hh"
#include <algorithm>
#include <boost/parser/parser.hpp>
#include <iterator>
#include <span>

template<typename T, size_t N>
T sum_all(std::span<T, N> l) {
    return std::ranges::fold_left(l, 0, std::plus<T>());
}

template<typename T, size_t N>
T prod_all(std::span<T, N> l) {
    return std::ranges::fold_left(l, 1, std::multiplies<T>());
}

size_t recurse(const std::string_view condition_report, const std::span<const unsigned long long> contiguous_damaged) {
    if (condition_report.empty()) {
        if (contiguous_damaged.empty()) {
            return 1;
        } else {
            return 0;
        }
    }
    if (contiguous_damaged.empty()) {
        if (std::ranges::all_of(condition_report, [](const auto c){ return c == '.' || c == '?'; })) {
            return 1;
        } else {
            return 0;
        }
    }
    const auto c = condition_report.front();
    const auto group_size = contiguous_damaged.front();
    if (condition_report.size() < group_size) {
        return 0;
    }
    if (c == '.') {
        std::cout << '.';
        return recurse(condition_report.substr(1), contiguous_damaged);
    } else if (c == '#') {
        std::cout << '#';
        if (std::ranges::all_of(
            condition_report.substr(0, group_size),
            [](const auto c){ return c == '?' || c == '#'; }
        )) {
            size_t result = 0;
            if (condition_report.size() == group_size) {
                result = 1;
            } else if (condition_report[group_size] == '.' || condition_report[group_size] == '?') {
                result = recurse(condition_report.substr(group_size + 1), contiguous_damaged.subspan(1));
            } else {
                result = 0;
            }
            return result;
        } else {
            return 0;
        }
    } else if (c == '?') {
        size_t result = 0;
        if (std::ranges::all_of(
            condition_report.substr(0, group_size),
            [](const auto c){ return c == '?' || c == '#'; }
        ) && (
            condition_report.size() == group_size ||
            condition_report[group_size] == '.' ||
            condition_report[group_size] == '?'
        )) {
            if (group_size + 1 <= condition_report.size()) {
                result += recurse(condition_report.substr(group_size + 1), contiguous_damaged.subspan(1));
            }
        }
        result += recurse(condition_report.substr(1), contiguous_damaged);
        return result;
    } else {
        return 0;
    }
}

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line = *bp::char_(".#?") >> bp::blank >> bp::ulong_long % ',' >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(???.### 1,1,3
.??..??...?##. 1,1,3
?#?#?#?#?#?#?#? 1,3,1,6
????.#...#... 4,1,1
????.######..#####. 1,6,5
?###???????? 3,2,1
)EOF"};
    const auto result = bp::parse(example, p_file);
    // const auto result = bp::parse(file_as_string(binary_name_to_test_file_name(argv[0])), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& input = *result;

    //part 1
    size_t sum = 0;
    for (const auto& [condition_report, contiguous_damaged]: input) {
        std::span<const unsigned long long> s = std::span(contiguous_damaged);
        size_t result = recurse(condition_report, s);
        std::cout << condition_report << " " << contiguous_damaged << " " << result << std::endl;
        sum += result;
    }
    std::cout << "part 1 = " << sum << std::endl;

    //part 2
    std::cout << "part 2 = " << 0 << std::endl;

    return 0;
}
