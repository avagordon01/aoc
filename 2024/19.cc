#include "util.hh"

template<size_t N>
struct trie_node {
    bool end{false};
    std::array<trie_node<N>*, N> children{nullptr};
};
template<size_t N = 5>
struct trie {
    trie_node<N> root;
    void insert(const std::vector<int>& s) {
        trie_node<N>* node = &root;
        for (const auto& i: s) {
            if (!node->children[i]) {
                node->children[i] = new trie_node<N>;
            }
            node = node->children[i];
        }
        node->end = true;
    }
    std::vector<std::span<int>> all_matches(const std::span<int>& s) {
        trie_node<N>* node = &root;
        std::vector<std::span<int>> out;
        size_t length = 0;
        if (node->end) {
            out.push_back(s.subspan(0, 0));
        }
        for (const auto& [length, i]: s | std::views::enumerate) {
            if (!node->children[i]) {
                break;
            }
            node = node->children[i];
            if (node->end) {
                out.push_back(s.subspan(0, length + 1));
            }
        }
        return out;
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::span<T>& v) {
    os << "span{";
    for (const auto& x: v) {
        os << x << ", ";
    }
    os << "}";
    return os;
}

bool recursive_match(trie<5>& t, std::span<int> s) {
    for (const auto& m: t.all_matches(s) | std::views::reverse) {
        if (m.size() == s.size()) {
            return true;
        }
        if (recursive_match(t, s.subspan(m.size()))) {
            return true;
        }
    }
    return false;
}

size_t sum_counts(const std::unordered_map<size_t, std::vector<size_t>>& to_counts, size_t pos, size_t size) {
    //to_counts[i] is the number of edges that lead directly into i
    //counts[i] is the number of ways to get to i
    std::vector<int> counts(size);
    for (size_t i = 0; i < size; i++) {
        auto it = to_counts.find(i);
        if (it == to_counts.end()) {
            continue;
        }
        for (const auto& l: it->second) {
            counts[i] += counts[i - l];
        }
    }
    return counts.back();
}

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_towel = +bp::symbols<int>{
        {"w", 0},
        {"u", 1},
        {"b", 2},
        {"r", 3},
        {"g", 4},
    };
    const auto p_available_towels = p_towel % ", " >> bp::eol >> bp::eol;
    const auto p_needed_towels = p_towel % bp::eol;
    const auto p_file = p_available_towels >> p_needed_towels >> bp::eol >> bp::eoi;

    const auto example = std::string{R"EOF()EOF"};
    //const auto result = bp::parse(example, p_file);
    auto result = bp::parse(file_as_string("2024/19.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    auto& input = *result;

    auto& [available, needed] = input;

    //construct trie
    auto root = trie<5>{};

    for (const auto& towel: available) {
        root.insert(towel);
    }

    size_t total = 0;
    for (auto& towel: needed) {
        if (recursive_match(root, towel)) {
            total++;
        }
    }
    std::cout << "part 1 = " << total << std::endl;

    size_t ways = 0;
    size_t i = 0;
    for (auto& towel: needed) {
        std::unordered_map<size_t, std::vector<size_t>> to_counts;
        for (const auto& [i, _]: towel | std::views::enumerate) {
            auto s = std::span<int>(towel).subspan(i);
            for (const auto& m: root.all_matches(s)) {
                to_counts[i + m.size()].push_back(i);
            }
        }
        ways += sum_counts(to_counts, 0, towel.size());
    }
    std::cout << "part 2 = " << ways << std::endl;

    return 0;
}
