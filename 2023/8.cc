#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_header = *bp::char_("RL") >> bp::eol;
    const auto p_line = *bp::upper >> " = (" >> *bp::upper >> "," >> bp::blank >> *bp::upper >> ")" >> bp::eol;
    const auto p_file = p_header >> bp::eol >> *p_line >> bp::eoi;

    const auto example = std::string{R"EOF()EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2023/8.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& input = *result;

    //part 1
    const auto& [header, rest] = input;
    std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>> map;
    for (const auto& [a, b, c]: rest) {
        map[a] = {b, c};
    }
    std::string_view pos = "AAA";
    size_t count = 0;
    while (pos != "ZZZ") {
        const auto c = header[count % header.size()];
        const auto& [l, r] = map[pos];
        if (c == 'L') {
            pos = l;
        } else if (c == 'R') {
            pos = r;
        } else {
        }
        count++;
    }
    std::cout << "part 1 = " << count << std::endl;

    //part 2
    const auto find_loop = [map, header](std::string_view start_pos){
        auto pos = start_pos;
        size_t i = 0;
        // NOTE this works because there's only one Z in each loop
        for (; pos.back() != 'Z'; i++) {
            const auto c = header[i % header.size()];
            const auto [l, r] = map.at(pos);
            if (c == 'L') {
                pos = l;
            } else if (c == 'R') {
                pos = r;
            } else {
            }
        }
        return i;
    };
    std::vector<size_t> v;
    for (const auto& [pos, b, c]: rest) {
        if (pos.back() == 'A') {
            const auto loop_pos = find_loop(pos);
            v.push_back(loop_pos);
        }
    }
    std::cout << "part 2 = " << *lcm(v) << std::endl;
    return 0;
}
