#include "util.hh"

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line = *bp::lower >> ':' >> bp::blank >> *bp::lower % bp::blank >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF(aaa: you hhh
you: bbb ccc
bbb: ddd eee
ccc: ddd eee fff
ddd: ggg
eee: out
fff: out
ggg: out
hhh: ccc fff iii
iii: out
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
    std::unordered_map<std::string, std::span<const std::string>> graph;
    for (const auto& line: input) {
        const auto& name = line.front();
        const auto outputs = std::span(line).subspan(1);
        graph[name] = outputs;
    }

    // breadth-first search
    std::unordered_map<std::string, size_t> paths_to_node;
    std::unordered_set<std::string> visited;
    std::deque<std::string> next_positions;

    const std::string start = "you";
    paths_to_node[start] = 1;
    next_positions.push_back(start);

    while (!next_positions.empty()) {
        const auto pos = next_positions.front();
        next_positions.pop_front();
        if (pos == "out") {
            continue;
        }
        for (const auto &next_position: graph[pos]) {
            paths_to_node[next_position] += 1;
            next_positions.push_back(next_position);
        }
    }
    std::cout << "part 1 = " << paths_to_node["out"] << std::endl;

    //part 2
    {
        const auto example = std::string{R"EOF(svr: aaa bbb
aaa: fft
fft: ccc
bbb: tty
tty: ccc
ccc: ddd eee
ddd: hub
hub: fff
eee: dac
dac: fff
fff: ggg hhh
ggg: out
hhh: out
)EOF"};
        // const auto result = bp::parse(example, p_file);
        // const auto& input = *result;

        std::unordered_map<std::string, std::unordered_set<std::string>> inv_graph;
        for (const auto& line: input) {
            const auto& name = line.front();
            for (const auto& output: std::span(line).subspan(1)) {
                inv_graph[output].insert(name);
            }
        }

        std::deque<std::tuple<std::string, bool, bool>> next_positions;
        std::unordered_map<std::string, std::array<size_t, 4>> paths_to_node_seen_dac_fft;
        std::unordered_set<std::string> visited;

        next_positions.push_back({"svr", false, false});
        visited.insert("svr");

        while (!next_positions.empty()) {
            auto [pos, dac_seen, fft_seen] = next_positions.front();
            next_positions.pop_front();
            visited.insert(pos);
            if (pos == "out") {
                continue;
            } else if (pos == "dac") {
                dac_seen = true;
            } else if (pos == "fft") {
                fft_seen = true;
            }
            for (const auto &next_position: graph[pos]) {
                paths_to_node_seen_dac_fft[next_position][(dac_seen << 0) | (fft_seen << 1)] += 1;
                if (visited.contains(next_position)) {
                    continue;
                }
                next_positions.push_back({next_position, dac_seen, fft_seen});
            }
        }
        size_t sum = paths_to_node_seen_dac_fft["out"][3];
        std::cout << "part 2 = " << sum << std::endl;
    }

    return 0;
}
