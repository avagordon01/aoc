#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto page_rule_pair = bp::transform(tuple_to_pair)[bp::ulong_long >> '|' >> bp::ulong_long >> bp::eol];
    const auto page_list = (bp::ulong_long % ',') >> bp::eol;
    const auto file = *page_rule_pair >> bp::ws >> *page_list >> *bp::ws;

    const auto result = bp::parse(file_as_string("2024/5.input.txt"), file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    auto& [page_rule_pairs, page_lists] = *result;

    //part 2
    size_t max_page = 0;
    for (const auto& page_list: page_lists) {
        for (size_t page: page_list) {
            max_page = std::max(page, max_page);
        }
    }
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
        boost::property<boost::vertex_color_t, size_t>,
        boost::property<boost::edge_index_t, size_t>> Graph;
    typedef boost::graph_traits<Graph>::vertex_descriptor size_t;
    Graph g(page_rule_pairs.begin(), page_rule_pairs.end(), max_page);

    //part 1
    std::unordered_map<size_t, std::unordered_set<size_t>> page_rules;
    for (auto [before, after]: page_rule_pairs) {
        page_rules[after].insert(before);
    }
    size_t middle_sum = 0;
    size_t ordered_middle_sum = 0;
    for (const auto& page_list: page_lists) {
        bool ordered = true;

        std::unordered_set<size_t> reject_pages;
        for (const auto& page: page_list) {
            if (reject_pages.contains(page)) {
                ordered = false;
                break;
            }
            for (const auto& x: page_rules[page]) {
                reject_pages.insert(x);
            }
        }
        if (ordered) {
            //part 1
            middle_sum += page_list[page_list.size() / 2];
        } else {
            //part 2
            std::unordered_set<size_t> pages;
            for (auto page: page_list) {
                pages.insert(page);
            }

            //NOTE
            //function object is necessary because a lambda operator= is deleted if it has any captures at all (even const ref)
            //https://en.cppreference.com/w/cpp/language/lambda#ClosureType::operator.3D.28const_ClosureType.26.29
            struct {
                bool operator()(Graph::edge_descriptor) const { return true; }
                bool operator()(Graph::vertex_descriptor vd) const { return pages->contains(vd); }
                std::unordered_set<size_t>* pages;
            } pred{&pages};
            boost::filtered_graph fg(g, pred, pred);

            std::vector<size_t> output;
            boost::topological_sort(fg, std::back_inserter(output));
            ordered_middle_sum += output[output.size() / 2];
        }
    }
    std::cout << "part 1 = " << middle_sum << std::endl;
    std::cout << "part 2 = " << ordered_middle_sum << std::endl;

    return 0;
}
