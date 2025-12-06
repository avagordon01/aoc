#include "util.hh"

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line = *bp::digit >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(987654321111111
811111111111119
234234234234278
818181911112111
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
    size_t sum = 0;
    for (const auto& bank: input) {
        const auto it_a = std::ranges::max_element(bank.begin(), bank.end() - 1);
        const auto it_b = std::ranges::max_element(it_a + 1, bank.end());
        ASSERT(it_a != bank.end());
        ASSERT(it_b != bank.end());
        size_t x = (*it_a - '0') * 10 + (*it_b - '0');
        sum += x;
    }
    std::cout << "part 1 = " << sum << std::endl;

    //part 2
    sum = 0;
    for (const auto& bank: input) {
        size_t number = 0;
        auto it = bank.begin();
        for (int n = 11; n >= 0; n--) {
            it = std::ranges::max_element(it, bank.end() - n);
            ASSERT(it != bank.end());
            number *= 10;
            number += *it - '0';
            it += 1;
        }
        sum += number;
    }
    std::cout << "part 2 = " << sum << std::endl;

    return 0;
}
