module;
//TODO
//use import std instead when it works
//https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html#import-std-support
#include <bits/stdc++.h>

#include <boost/parser/parser.hpp>
#include <boost/hana.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/container_hash/is_tuple_like.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/hawick_circuits.hpp>
#include <boost/graph/bron_kerbosch_all_cliques.hpp>

#include <Eigen/Dense>

#include <libassert/assert.hpp>

// #include <coin/ClpSimplex.hpp>
// #include <coin/CbcModel.hpp>

export module utilmod;

auto binary_name_to_test_file_name(const char* binary) -> std::string {
    // "./install/bin/2024-1"
    // "2024-1.input.txt"
    auto s = std::string(binary);
    s.erase(0, s.rfind('/') + 1);
    s[s.find('-')] = '/';
    s.append(".input.txt");
    return s;
};

auto file_as_string(std::string filename) -> std::string {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

auto get_or(const auto& container, const auto& key, const auto& default_) -> decltype(default_) {
    if (container.find(key) == container.end()) {
        return default_;
    } else {
        return container.find(key)->second;
    }
}

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

const auto tuple_to_pair = [](const auto& t) {
    auto& [a, b] = t;
    return std::make_pair(a, b);
};

//multi-dimensional bounds checking
const auto md_bounds_check = [](const auto& bounds, const auto& index) -> bool {
    return (index.array() >= 0 && index.array() < bounds.array()).all();
};

//implements std::hash for any range / container
namespace std {
    template <ranges::range T>
    struct hash<T>
    {
    size_t operator()(const T& x) const
    {
        return boost::hash_range(x.begin(), x.end());
    }
    };
}

//implements std::hash for pair, tuple, array, etc
namespace std {
    template <typename T>
    requires boost::container_hash::is_tuple_like<T>::value
    struct hash<T>
    {
    size_t operator()(const T& x) const
    {
        return boost::hash_value(x);
    }
    };
}

//grid container printing
void print_grid(std::ostream& os, const auto& grid) {
    for (const auto& row: grid) {
        for (char c: row) {
            if (std::isprint(c)) {
                os << c;
            } else {
                os << ' ';
            }
        }
        os << std::endl;
    }
}
template<size_t N, size_t M>
std::ostream& operator<<(std::ostream& os, const std::array<std::array<char, M>, N>& grid) {
    print_grid(os, grid);
    return os;
}
std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<char>>& grid) {
    print_grid(os, grid);
    return os;
}
std::ostream& operator<<(std::ostream& os, const std::span<std::span<char>>& grid) {
    print_grid(os, grid);
    return os;
}
std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& grid) {
    print_grid(os, grid);
    return os;
}

//std container printing
namespace util {
template<typename X, typename Y>
std::ostream& operator<<(std::ostream& os, const std::pair<X, Y>& p) {
    os << "pair{" << p.first << ", " << p.second << "}";
    return os;
}

template<typename X>
std::ostream& operator<<(std::ostream& os, const std::vector<X>& v) {
    os << "vector{";
    for (const auto& x: v) {
        os << x << ", ";
    }
    os << "}";
    return os;
}

template<typename X, size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<X, N>& a) {
    os << "array{";
    for (const auto& x: a) {
        os << x << ", ";
    }
    os << "}";
    return os;
}

template<typename X>
std::ostream& operator<<(std::ostream& os, const std::list<X>& l) {
    os << "list{";
    for (const auto& x: l) {
        os << x << ", ";
    }
    os << "}";
    return os;
}

template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V>& m) {
    os << "unordered_map{";
    for (const auto& [k, v]: m) {
        os << "{" << k << ": " << v << "}, ";
    }
    os << "}";
    return os;
}

template<typename V>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<V>& s) {
    os << "unordered_set{";
    for (const auto& v: s) {
        os << v << ", ";
    }
    os << "}";
    return os;
}

template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m) {
    os << "map{";
    for (const auto& [k, v]: m) {
        os << "{" << k << ": " << v << "}, ";
    }
    os << "}";
    return os;
}

template<typename V>
std::ostream& operator<<(std::ostream& os, const std::set<V>& s) {
    os << "set{";
    for (const auto& v: s) {
        os << v << ", ";
    }
    os << "}";
    return os;
}
}

template<size_t X, size_t Y, size_t Z = 1>
struct mdbitset: std::bitset<X * Y * Z> {
    using parent = std::bitset<X * Y * Z>;
    using reference = parent::reference;
    constexpr reference operator[](Eigen::Vector2i pos) {
        assert((pos.array() >= 0).all());
        return operator[](pos[0], pos[1]);
    }
    constexpr reference operator[](size_t x, size_t y, size_t z) {
        assert(x < X && y < Y && z < Z);
        return parent::operator[](z * Y * X + y * X + x);
    }
    constexpr bool operator[](size_t x, size_t y, size_t z) const {
        assert(x < X && y < Y && z < Z);
        return parent::operator[](z * Y * X + y * X + x);
    }
    constexpr reference operator[](size_t x, size_t y) {
        assert(x < X && y < Y);
        return parent::operator[](y * X + x);
    }
    constexpr bool operator[](size_t x, size_t y) const {
        assert(x < X && y < Y);
        return parent::operator[](y * X + x);
    }
};

template<size_t X, size_t Y, size_t Z>
std::ostream& operator<<(std::ostream& os, const mdbitset<X, Y, Z>& b) {
    for (size_t y = 0; y < Y; y++) {
        for (size_t x = 0; x < X; x++) {
            os << (b[x, y] ? '#' : '.');
        }
        os << std::endl;
    }
    return os;
}

const auto dirs_2d_4 = std::array<Eigen::Vector2i, 4>{{
    {0, 1},
    {1, 0},
    {0, -1},
    {-1, 0},
}};

const auto dirs_2d_8 = std::array<Eigen::Vector2i, 8>{{
    {0, 1},
    {1, 1},
    {1, 0},
    {1, -1},
    {0, -1},
    {-1, -1},
    {-1, 0},
    {-1, 1},
}};

const auto dirs_3d_6 = std::array<Eigen::Vector3i, 6>{{
    {0, 0, 1},
    {0, 1, 0},
    {1, 0, 0},
    {0, 0, -1},
    {0, -1, 0},
    {-1, 0, 0},
}};

// mathematical operations
int sign(auto x) {
    if (x > 0) {
        return 1;
    } else if (x < 0) {
        return -1;
    } else {
        return 0;
    }
}

auto positive_mod(const auto a, const auto b) {
    return (b + (a % b)) % b;
}

template<typename T>
std::optional<T> lcm(const std::vector<T>& arr) {
    return std::ranges::fold_left_first(arr, &std::lcm<T, T>);
}

template<typename T>
std::optional<T> gcd(const std::vector<T>& arr) {
    return std::ranges::fold_left_first(arr, &std::gcd<T, T>);
}
