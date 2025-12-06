#include "util.hh"

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line_f = bp::ulong_long >> '-' >> bp::ulong_long >> bp::eol;
    const auto p_line_i = bp::ulong_long >> bp::eol;
    const auto p_file = *p_line_f >> bp::eol >> *p_line_i >> bp::eoi;

    const auto example = std::string{R"EOF(3-5
10-14
16-20
12-18

1
5
8
11
17
32
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
    auto [fresh_intervals, ingredients] = input;
    for (auto& a: fresh_intervals) {
        auto& [a0, a1] = a;
        if (a0 < a1) {
        } else {
            std::swap(a0, a1);
        }
    }
    std::ranges::sort(fresh_intervals, [](const auto a, const auto b){
        const auto [a0, a1] = a;
        const auto [b0, b1] = b;
        return a0 < b0 || (a0 == b0 && a1 < b1);
    });
    size_t i, j;
    for (i = 0, j = 1; i < fresh_intervals.size() && j < fresh_intervals.size();) {
        auto& [a0, a1] = fresh_intervals[i];
        auto& [b0, b1] = fresh_intervals[j];
        if (a1 >= b0 - 1) {
            a1 = std::max(a1, b1);
        } else {
            i++;
            fresh_intervals[i] = fresh_intervals[j];
        }
        j++;
    }
    fresh_intervals.erase(fresh_intervals.begin() + i + 1, fresh_intervals.end());
    std::ranges::sort(ingredients);
    size_t fresh_count = 0;
    for (size_t i = 0, j = 0; i < ingredients.size();) {
        const auto a = ingredients[i];
        const auto [b0, b1] = fresh_intervals[j];
        if (a < b0) {
            i++;
        } else if (a >= b0 && a <= b1) {
            fresh_count++;
            i++;
        } else if (a > b1) {
            if (j < fresh_intervals.size() - 1) {
                j++;
            } else {
                break;
            }
        } else {
            ASSERT(false);
        }
    }
    std::cout << "part 1 = " << fresh_count << std::endl;

    //part 2
    size_t fresh_range = 0;
    for (const auto& [a, b]: fresh_intervals) {
        fresh_range += b - a + 1;
    }
    std::cout << "part 2 = " << fresh_range << std::endl;

    return 0;
}
