#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_name = +(bp::digit | bp::lower);
    const auto p_init = p_name >> ": " >> bp::int_ >> bp::eol;
    const auto p_op = bp::string("XOR") | bp::string("OR") | bp::string("AND");
    const auto p_rule = p_name >> ' ' >> p_op >> ' ' >> p_name >> " -> " >> p_name >> bp::eol;
    const auto p_file = +p_init >> bp::eol >> +p_rule >> bp::eoi;

    const auto example = std::string{R"EOF()EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2024/24.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& input = *result;

    const auto& [inits, rules] = input;
    std::cout << inits.size() << ", " << rules.size() << std::endl;

    std::unordered_map<std::string, bool> variables;
    for (const auto& [name, value]: inits) {
        variables[name] = value;
    }

    auto rules_deque = std::ranges::to<std::deque<std::tuple<std::string, std::string, std::string, std::string>>>(rules);
    while (!rules_deque.empty()) {
        const auto rule = rules_deque.front();
        const auto [a, op, b, c] = rule;
        rules_deque.pop_front();

        if (variables.find(a) != variables.end() && variables.find(b) != variables.end()) {
            if (op == "OR") {
                variables[c] = variables[a] | variables[b];
            } else if (op == "AND") {
                variables[c] = variables[a] & variables[b];
            } else if (op == "XOR") {
                variables[c] = variables[a] ^ variables[b];
            }
        } else {
            rules_deque.push_back(rule);
        }
    }

    std::bitset<64> z;
    for (const auto& [name, value]: variables) {
        if (name.starts_with("z")) {
            const auto bit_index = std::stoi(name.substr(1));
            std::cout << "bit_index = " << bit_index << std::endl;
            z.set(bit_index, value);
        }
    }

    //part 1
    std::cout << "part 1 = " << z.to_ullong() << std::endl;

    //part 2
    std::cout << "part 2 = " << 0 << std::endl;

    return 0;
}
