#include "util.hh"

std::pair<char, char> int_to_name(size_t x) {
    return std::make_pair(
        x / 26 + 'a',
        x % 26 + 'a'
    );
}
size_t name_to_int(std::pair<char, char> x) {
    return (x.first - 'a') * 26 + (x.second - 'a');
}

struct cycle_visitor {
    size_t& count;
    template<typename Path, typename Graph>
    void cycle(const Path& p, const Graph& g) {
        if (p.size() == 3) {
            for (const auto& v: p) {
                size_t i = boost::get(boost::vertex_index, g, v);
                const auto name = int_to_name(i);
                std::cout << name.first << name.second << ",";
                if (int_to_name(i).first == 't') {
                    count++;
                    break;
                }
            }
            std::cout << std::endl;
        }
    }
};

struct clique_visitor {
    std::deque<size_t>& largest_clique;

    template<typename Clique, typename Graph>
    void clique(const Clique& p, const Graph& g) {
        if (p.size() > largest_clique.size()) {
            largest_clique = p;
        }
    }
};

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_computer = bp::transform(tuple_to_pair)[bp::separate[bp::lower >> bp::lower]];
    const auto p_connection = p_computer >> '-' >> p_computer;
    const auto p_line = p_connection >> bp::eol;
    const auto p_file = +p_line >> bp::eoi;

    const auto example = std::string{R"EOF(kh-tc
qp-kh
de-cg
ka-co
yn-aq
qp-ub
cg-tb
vc-aq
tb-ka
wh-tc
yn-cg
kh-ub
ta-co
de-co
tc-td
tb-wq
wh-td
ta-ka
td-qp
aq-cg
wq-ub
ub-vc
de-ta
wq-aq
wq-vc
wh-yn
ka-de
kh-ta
co-tc
wh-qp
tb-vc
td-yn
)EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2024/23.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& input = *result;

    const auto max_index = name_to_int(std::make_pair('z', 'z'));
    std::vector<std::pair<size_t, size_t>> connections;
    for (const auto& [a, b]: input) {
        connections.push_back(std::make_pair(name_to_int(a), name_to_int(b)));
    }

    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
    Graph g(connections.begin(), connections.end(), max_index);

    size_t count = 0;
    cycle_visitor cv{count};

    //std::cout << "V = " << g.vertex_set().size() << std::endl;
    //std::cout << "E = " << g.m_edges.size() << std::endl;

    //restricting max circuit length to 3 is required to be able to complete in a reasonable time
    boost::hawick_circuits(g, cv, boost::get(boost::vertex_index, g), 3);
    count /= 2; // ignore opposite directions

    //part 1
    std::cout << "part 1 = " << count << std::endl;

    std::deque<size_t> largest_clique;
    clique_visitor clv{largest_clique};
    bron_kerbosch_all_cliques(g, clv);

    std::ranges::sort(largest_clique);

    std::string output;
    for (const auto& i: largest_clique) {
        const auto p = int_to_name(i);
        output += p.first;
        output += p.second;
        output += ',';
    }
    output.pop_back();

    //part 2
    std::cout << "part 2 = " << output << std::endl;

    return 0;
}
