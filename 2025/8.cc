#include "util.hh"
#include <boost/parser/parser.hpp>

#include <libmorton/morton.h>

template<typename T, int D>
std::pair<Eigen::Vector<T, D>, Eigen::Vector<T, D>>
pow2_bounding_box(Eigen::Vector<T, D> a, Eigen::Vector<T, D> b) {
    size_t msb = 0;
    for (size_t d = 0; d < D; d++) {
        size_t diff = a[d] ^ b[d];
        size_t m = std::numeric_limits<size_t>::digits - std::countl_zero(diff);
        msb = std::max(msb, m);
    }
    const size_t mask = (1 << msb) - 1;
    Eigen::Vector<T, D> high, low;
    for (size_t d = 0; d < D; d++) {
        low[d] = a[d] & ~mask;
        high[d] = a[d] | mask;
    }
    return {low, high};
}

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line = bp::ulong_long >> ',' >> bp::ulong_long >> ',' >> bp::ulong_long >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(162,817,812
57,618,57
906,360,560
592,479,940
352,342,300
466,668,158
542,29,236
431,825,988
739,650,466
52,470,668
216,146,977
819,987,18
117,168,530
805,96,715
346,949,466
970,615,88
941,993,340
862,61,35
984,92,344
425,690,689
)EOF"};
    // const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string(binary_name_to_test_file_name(argv[0])), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& input = *result;

    //tests
    {
        using vec = Eigen::Vector<size_t, 2>;
        ASSERT(pow2_bounding_box(vec(61, 61), vec(127, 127)) == std::make_pair(vec(0, 0), vec(127, 127)));
        ASSERT(pow2_bounding_box(vec(0, 0), vec(1, 1)) == std::make_pair(vec(0, 0), vec(1, 1)));
        ASSERT(pow2_bounding_box(vec(0, 1), vec(1, 0)) == std::make_pair(vec(0, 0), vec(1, 1)));
        ASSERT(pow2_bounding_box(vec(1, 0), vec(1, 1)) == std::make_pair(vec(0, 0), vec(1, 1)));
        ASSERT(pow2_bounding_box(vec(1, 1), vec(0, 1)) == std::make_pair(vec(0, 0), vec(1, 1)));
    }

    //part 1
    using vec = Eigen::Vector<size_t, 3>;
    std::vector<vec> positions;
    for (const auto& [x, y, z]: input) {
        positions.push_back(vec(x, y, z));
    }
    const auto morton = [](const auto position){
        return libmorton::morton3D_64_encode(position[0], position[1], position[2]);
    };
    std::ranges::sort(positions, std::less(), morton);
    std::vector<std::tuple<size_t, size_t, size_t>> pairwise_distances;
    for (size_t i = 0; i < positions.size(); i++) {
        for (size_t j = i + 1; j < positions.size(); j++) {
            const auto a = positions[i];
            const auto b = positions[j];
            size_t distance = (a - b).squaredNorm();
            pairwise_distances.push_back({i, j, distance});
        }
    }
    std::ranges::sort(pairwise_distances, std::less(), [](const auto a){ return std::get<2>(a); });
    // size_t n = 10;
    size_t n = 1000;
    // TODO use a bi-map instead
    // TODO use a graph instead
    std::unordered_map<size_t, size_t> circuits;
    size_t circuit_id = 0;
    for (size_t i = 0; i < n; i++) {
        const auto [a, b, distance] = pairwise_distances[i];
        bool found = false;
        if (circuits.contains(a) && circuits.contains(b)) {
            size_t a_id = circuits[a];
            size_t b_id = circuits[b];
            for (auto& [_, circuit_id]: circuits) {
                if (circuit_id == b_id) {
                    circuit_id = a_id;
                }
            }
        } else if (circuits.contains(a)) {
            circuits[b] = circuits[a];
        } else if (circuits.contains(b)) {
            circuits[a] = circuits[b];
        } else {
            circuits[a] = circuit_id;
            circuits[b] = circuit_id;
            circuit_id++;
        }
    }
    std::vector<size_t> circuit_sizes(circuit_id, 0);
    for (const auto& [_, circuit_id]: circuits) {
        circuit_sizes[circuit_id]++;
    }
    std::ranges::sort(circuit_sizes, std::greater());
    size_t product = 1;
    for (size_t i = 0; i < 3; i++) {
        const auto circuit_size = circuit_sizes[i];
        if (circuit_size != 0) {
            std::cout << circuit_size << std::endl;
            product *= circuit_size;
        }
    }
    std::cout << "part 1 = " << product << std::endl;

    //part 2
    std::cout << "part 2 = " << 0 << std::endl;

    return 0;
}
