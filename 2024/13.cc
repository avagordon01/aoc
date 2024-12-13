#include "util.hh"

template<typename T, int N>
Eigen::Vector<T, N> array_to_eigen(std::array<T, N> a) {
    return Eigen::Vector<T, N>(a.data());
}

template<typename tuple_t>
constexpr auto tuple_to_array(tuple_t&& tuple) {
    constexpr auto get_array = [](auto&& ... x){ return std::array{std::forward<decltype(x)>(x) ... }; };
    return std::apply(get_array, std::forward<tuple_t>(tuple));
}

template<typename T, int N, typename tuple_t>
constexpr auto tuple_to_eigen(tuple_t&& tuple) {
    return array_to_eigen<T, N>(tuple_to_array(tuple));
}

template<typename T, int N>
double angle_between(const Eigen::Vector<T, N>& a, const Eigen::Vector<T, N>& b) {
    return std::acos(a.dot(b) / (a.norm() * b.norm()));
}

template<typename T, int N>
double parallel(const Eigen::Vector<T, N>& a, const Eigen::Vector<T, N>& b) {
    return pow(a.dot(b), 2) == a.squaredNorm() * b.squaredNorm();
}

auto main() -> int {
    namespace bp = boost::parser;

    using coord = Eigen::Vector2d;

    const auto tuple_to_coord = [](const auto& tuple) {
        const auto [a, b] = tuple;
        return coord(a, b);
    };

    const auto p_puzzle =
        "Button A: X+" >> bp::int_ >> ", Y+" >> bp::int_ >> bp::eol >>
        "Button B: X+" >> bp::int_ >> ", Y+" >> bp::int_ >> bp::eol >>
        "Prize: X=" >> bp::int_ >> ", Y=" >> bp::int_ >> *bp::eol;
    const auto p_file = *p_puzzle >> bp::eoi;

    const auto example = std::string{R"EOF(Button A: X+94, Y+34
Button B: X+22, Y+67
Prize: X=8400, Y=5400

Button A: X+26, Y+66
Button B: X+67, Y+21
Prize: X=12748, Y=12176

Button A: X+17, Y+86
Button B: X+84, Y+37
Prize: X=7870, Y=6450

Button A: X+69, Y+23
Button B: X+27, Y+71
Prize: X=18641, Y=10279

)EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2024/13.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& puzzles = *result;

    std::vector<std::array<coord, 3>> puzzles_vec;
    for (const auto& puzzle: puzzles) {
        const auto& [a, b, c, d, e, f] = puzzle;
        puzzles_vec.push_back({coord(a, b), coord(c, d), coord(e, f)});
    }

    //part 1
    size_t total = 0;
    for (const auto& [a, b, x]: puzzles_vec) {
        ASSERT(!(parallel(a, b) || parallel(a, x) || parallel(b, x)));
        const auto angle_a_x = angle_between(a, x);
        const auto angle_a_b = angle_between(a, b);
        const auto angle_b_x = angle_between(b, x);

        const auto length_x = x.norm();
        const auto length_a = sin(angle_b_x) * length_x / sin(angle_a_b);
        const auto length_b = sin(angle_a_x) * length_x / sin(angle_a_b);

        const auto n_a = llround(length_a / a.norm());
        const auto n_b = llround(length_b / b.norm());

        const auto sum = n_a * a + n_b * b;
        if (sum == x) {
            total += n_a * 3 + n_b;
        }
    }
    std::cout << "part 1 = " << total << std::endl;

    //part 2
    size_t total_p2 = 0;
    for (auto& [a, b, x]: puzzles_vec) {
        x += coord(10000000000000, 10000000000000);
        ASSERT(!(parallel(a, b) || parallel(a, x) || parallel(b, x)));
        const auto angle_a_x = angle_between(a, x);
        const auto angle_a_b = angle_between(a, b);
        const auto angle_b_x = angle_between(b, x);

        const auto length_x = x.norm();
        const auto length_a = sin(angle_b_x) * length_x / sin(angle_a_b);
        const auto length_b = sin(angle_a_x) * length_x / sin(angle_a_b);

        const auto n_a = llround(length_a / a.norm());
        const auto n_b = llround(length_b / b.norm());

        const auto sum = n_a * a + n_b * b;
        if (sum == x) {
            total_p2 += n_a * 3 + n_b;
        }
    }
    std::cout << "part 2 = " << total_p2 << std::endl;

    return 0;
}
