#include "util.hh"

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line_1 = *bp::blank >> (bp::ulong_long % +bp::blank) >> *bp::blank >> bp::eol;
    const auto p_line_2 = *bp::blank >> (bp::char_("+*") % +bp::blank) >> *bp::blank >> bp::eol;
    const auto p_file = *p_line_1 >> p_line_2 >> bp::eoi;

    const auto example = std::string{R"EOF(123 328  51 64 
 45 64  387 23 
  6 98  215 314
*   +   *   +  
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
    const auto& [nums, ops] = input;
    size_t w = nums[0].size();
    size_t h = nums.size();
    std::vector<size_t> sums(ops.size(), 0);
    for (size_t i = 0; i < ops.size(); i++) {
        if (ops[i] == '+') {
            sums[i] = 0;
        } else if (ops[i] == '*') {
            sums[i] = 1;
        } else {
            ASSERT(false);
        }
    }
    for (size_t i = 0; i < ops.size(); i++) {
        auto& sum = sums[i];
        const auto& op = ops[i];
        for (size_t j = 0; j < nums.size(); j++) {
            const auto& num = nums[j][i];
            if (op == '+') {
                sum += num;
            } else if (op == '*') {
                sum *= num;
            } else {
                ASSERT(false);
            }
        }
    }
    size_t sum = 0;
    for (size_t i = 0; i < sums.size(); i++) {
        sum += sums[i];
    }
    std::cout << "part 1 = " << sum << std::endl;

    //part 2
    {
        const auto p_line_1 = *bp::char_("0123456789 ") >> bp::eol;
        const auto p_line_2 = *bp::char_("+* ") >> bp::eol;
        const auto p_file = bp::separate[*p_line_1 >> p_line_2] >> bp::eoi;
        // const auto result = bp::parse(example, p_file);
        const auto result = bp::parse(file_as_string(binary_name_to_test_file_name(argv[0])), p_file);
        if (!result) {
            std::cerr << "failed to parse input" << std::endl;
            return 1;
        }

        const auto& input = *result;

        const auto& [nums, ops] = input;
        const auto fn = [nums, ops](size_t start, size_t end){
            const auto op = ops[start];
            ASSERT(op == '+' || op == '*');
            size_t sum = op == '+' ? 0 : 1;
            for (size_t i = start; i < end; i++) {
                std::string num_str;
                for (size_t j = 0; j < nums.size(); j++) {
                    const auto c = nums[j][i];
                    num_str.push_back(c);
                }
                size_t num = std::stoull(num_str);
                if (op == '+') {
                    sum += num;
                } else if (op == '*') {
                    sum *= num;
                }
            }
            return sum;
        };
        size_t sum = 0;
        for (size_t i = 0, j = 1; i < ops.size();) {
            ASSERT(ops[i] == '+' || ops[i] == '*');
            if (ops[j] == '+' || ops[j] == '*' || j == ops.size()) {
                if (j == ops.size()) {
                    j += 1;
                }
                sum += fn(i, j - 1);
                i = j;
                j++;
            } else {
                j++;
            }
        }
        std::cout << "part 2 = " << sum << std::endl;
    }

    return 0;
}
