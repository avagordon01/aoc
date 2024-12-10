//TODO
//use import std instead when it works
//https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html#import-std-support
#include <bits/stdc++.h>

#include <boost/parser/parser.hpp>
#include <boost/hana.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/filtered_graph.hpp>

#include <eigen3/Eigen/Dense>

#include <libassert/assert.hpp>

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

int sign(auto x) {
    if (x > 0) {
        return 1;
    } else if (x < 0) {
        return -1;
    } else {
        return 0;
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
    for (const auto& [b, i]: std::views::zip(bounds, index)) {
        if (i >= 0 && i < b) {
        } else {
            return false;
        }
    }
    return true;
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
