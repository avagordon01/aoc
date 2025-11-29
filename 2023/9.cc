#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_line = bp::long_long % bp::blank >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF()EOF"};
    // const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2023/9.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& input = *result;

    //part 1
    const auto find_sum = [](auto arr){
        bool all_zero = false;
        std::vector<int64_t> last_values;
        last_values.push_back(arr.back());
        do {
            auto d = arr |
                std::views::adjacent_transform<2>(
                    [](auto a, auto b){ return b - a; }
                );
            arr = std::ranges::to<std::vector>(d);
            last_values.push_back(arr.back());
            all_zero = std::ranges::all_of(arr, [](auto a){ return a == 0; });
        } while (!all_zero);
        const auto sum = std::ranges::fold_left(last_values, 0, std::plus<size_t>());
        return sum;
    };
    int64_t sum = 0;
    for (const auto& line: input) {
        sum += find_sum(line);
    }
    std::cout << "part 1 = " << sum << std::endl;

    //part 2
    const auto find_sum_backward = [](auto arr){
        bool all_zero = false;
        std::vector<int64_t> first_values;
        first_values.push_back(arr.front());
        do {
            auto d = arr |
                std::views::adjacent_transform<2>(
                    [](auto a, auto b){ return b - a; }
                );
            arr = std::ranges::to<std::vector>(d);
            first_values.push_back(arr.front());
            all_zero = std::ranges::all_of(arr, [](auto a){ return a == 0; });
        } while (!all_zero);
        first_values.pop_back();
        int64_t x = 0;
        for (const auto y: first_values | std::views::reverse) {
            x = y - x;
        }
        return x;
    };
    sum = 0;
    for (const auto& line: input) {
        sum += find_sum_backward(line);
    }
    std::cout << "part 2 = " << sum << std::endl;

    return 0;
}
