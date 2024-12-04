#include <boost/parser/parser.hpp>
#include <boost/hana.hpp>
#include <vector>
#include <unordered_map>
#include <cmath>
#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;
    using namespace boost::parser::literals;

    size_t sum_p1 = 0;
    size_t sum_p2 = 0;
    bool enabled = true;

    auto do_ = bp::lit("do()");
    auto dont = bp::lit("don't()");
    auto mul = "mul(" >> bp::long_long >> ',' >> bp::long_long >> ')';
    auto a_mul = [&](const auto& ctx) {
        auto [a, b] = _attr(ctx);
        sum_p1 += a * b;
        if (enabled) {
            sum_p2 += a * b;
        }
    };
    auto a_do = [&](const auto& ctx) { enabled = true; };
    auto a_dont = [&](const auto& ctx) { enabled = false; };
    auto parser = mul | do_ | dont;
    auto a_parser = mul[a_mul] | do_[a_do] | dont[a_dont];

    auto junk = bp::omit[*(bp::char_ - parser)];
    auto middle_parser = junk >> *(a_parser >> junk);

    auto result = bp::parse(file_as_string("3.input.txt"), middle_parser);

    std::cout << "sum_p1 = " << sum_p1 << std::endl;
    std::cout << "sum_p2 = " << sum_p2 << std::endl;

    return 0;
}
