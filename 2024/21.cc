#include "util.hh"

using coord = Eigen::Vector2i;

enum class keypad_type {
    numpad,
    dirpad,
};

const std::array<std::array<char, 3>, 2> dirpad_layout = {{
    {  0, '^', 'A'},
    {'<', 'v', '>'},
}};

const std::unordered_map<char, coord> dirpad_locations = {{
    {  0, coord(0, 0)},
    {'^', coord(1, 0)},
    {'A', coord(2, 0)},
    {'<', coord(0, 1)},
    {'v', coord(1, 1)},
    {'>', coord(2, 1)},
}};

const std::array<std::array<char, 3>, 4> numpad_layout = {{
    {'7', '8', '9'},
    {'4', '5', '6'},
    {'1', '2', '3'},
    {  0, '0', 'A'},
}};

const std::unordered_map<char, coord> numpad_locations = {{
    {'7', coord(0, 0)},
    {'8', coord(1, 0)},
    {'9', coord(2, 0)},
    {'4', coord(0, 1)},
    {'5', coord(1, 1)},
    {'6', coord(2, 1)},
    {'1', coord(0, 2)},
    {'2', coord(1, 2)},
    {'3', coord(2, 2)},
    {  0, coord(0, 3)},
    {'0', coord(1, 3)},
    {'A', coord(2, 3)},
}};

const std::unordered_map<coord, char> dir_to_char = {{
    {coord(0, 1), 'v'},
    {coord(1, 0), '>'},
    {coord(0, -1), '^'},
    {coord(-1, 0), '<'},
}};

struct keypad {
    const std::unordered_map<char, coord>& locations;
    coord pos;
    keypad(keypad_type kt):
        locations(kt == keypad_type::numpad ? numpad_locations : dirpad_locations),
        pos(start_pos())
    {}
    /*
    void move(char m) {
        coord dir;
        if (m == '^') {
            dir = coord(1, 0);
        } else if (m == 'v') {
            dir = coord(1, 0);
        } else if (m == '<') {
            dir = coord(1, 0);
        } else if (m == '>') {
            dir = coord(1, 0);
        } else {
            ASSERT(false, m);
        }
        pos += dir;
        md_bounds_check(size(), pos);
        ASSERT(pos != avoid_pos());
    }
    */
    void find_path(std::string& out, char c) {
        const auto target = locations.at(c);
        std::array<int, 2> order = {0, 1};
        if (target[0] == avoid_pos()[0] && pos[1] == avoid_pos()[1]) {
            order = {1, 0};
            //go vertically first, then horizontally second
        } else if (target[1] == avoid_pos()[1] && pos[0] == avoid_pos()[0]) {
            order = {0, 1};
            //go horizontally first, then vertically second
        } else {
            //either order is fine
            //FIXME why does the order make a difference to the length of the sequence?
            //because using the same order as the previous target would mean upper dirpads dont move as much
        }
        if (c == '<' || c == 'v') {
            order = {1, 0};
        }
        for (const auto i: order) {
            const auto d = target - pos;
            size_t n = d.array().abs()[i];
            if (n == 0) {
                continue;
            }
            const coord dir = coord::Unit(i).array() * d.array().sign();
            ASSERT(dir != coord::Zero());
            char c = dir_to_char.at(dir);
            for (size_t j = 0; j < n; j++) {
                out.push_back(c);
                pos += dir;
            }
        }
        out.push_back('A');
        ASSERT(pos == target);
    }
    std::string input(std::string_view in) {
        std::string out;
        for (const auto& c: in) {
            find_path(out, c);
        }
        return out;
    }
    coord start_pos() const {
        return locations.at('A');
    }
    coord avoid_pos() const {
        return locations.at(0);
    }
};

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_line = +bp::char_("0123456789A") >> bp::eol;
    const auto p_file = +p_line >> bp::eoi;

    const auto example = std::string{R"EOF(029A
980A
179A
456A
379A
)EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2024/21.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& input = *result;

    {
        for (const auto& input: {"369", "321"}) {
            keypad t_a_0(keypad_type::numpad);
            keypad t_a_1(keypad_type::dirpad);
            keypad t_a_2(keypad_type::dirpad);
            const auto v_a =
            t_a_1.input(
            t_a_0.input(input)
            );
            keypad t_b_0(keypad_type::numpad);
            keypad t_b_1(keypad_type::dirpad);
            keypad t_b_2(keypad_type::dirpad);
            const auto v_b =
            t_b_1.input(
            t_b_0.input(input)
            );
            ASSERT(v_a.size() == v_b.size(), v_a, v_b);
        }
    }
    {
        keypad t_a(keypad_type::dirpad);
        keypad t_b(keypad_type::dirpad);
        ASSERT(t_a.input("<^A").size() == t_b.input("^<A").size());
    }
    {
        keypad t_a(keypad_type::dirpad);
        keypad t_b(keypad_type::dirpad);
        ASSERT(t_a.input("<vvA").size() < t_b.input("v<vA").size());
    }
    /*
    {  0, '^', 'A'},
    {'<', 'v', '>'},

    {'7', '8', '9'},
    {'4', '5', '6'},
    {'1', '2', '3'},
    {  0, '0', 'A'},
    */
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("0") == std::string("<A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("1") == std::string("^<<A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("4") == std::string("^^<<A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("7") == std::string("^^^<<A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("3") == std::string("^A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("6") == std::string("^^A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("9") == std::string("^^^A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("2") == std::string("<^A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("5") == std::string("<^^A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("8") == std::string("<^^^A"));
    }
    #if 0
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("3") == std::string("^A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("0") == std::string("<A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("3") == std::string("^A"));
        ASSERT(t.input("0") == std::string("<vA"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("0") == std::string("<A"));
        ASSERT(t.input("3") == std::string(">^A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("4") == std::string("^^<<A"));
        ASSERT(t.input("7") == std::string("^A"));
        ASSERT(t.input("8") == std::string(">A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("7") == std::string("^^^<<A"));
    }
    {
        keypad t(keypad_type::numpad);
        ASSERT(t.input("8") == std::string("<^^^A"));
    }
    {
        keypad t(keypad_type::dirpad);
        ASSERT(t.input("<") == std::string("v<<A"));
    }
    {
        keypad t(keypad_type::dirpad);
        ASSERT(t.input("v") == std::string("<vA"));
    }
    {
        keypad t(keypad_type::dirpad);
        ASSERT(t.input(">") == std::string("vA"));
    }
    {
        keypad t(keypad_type::dirpad);
        ASSERT(t.input("^") == std::string("<A"));
    }
    #endif

    keypad a(keypad_type::dirpad);
    keypad b(keypad_type::dirpad);
    keypad c(keypad_type::numpad);


    size_t total = 0;
    for (const auto& line: input) {
        const auto num = std::stoi(line);
        const auto v = 
            a.input(
                b.input(
                    c.input(line)
                )
            );
        std::cout << line << ": " << v.size() << ", " << v << std::endl;
        total += num * v.size();
    }

    //part 1
    std::cout << "part 1 = " << total << std::endl;
    ASSERT(total < 170862);

    //part 2
    std::cout << "part 2 = " << 0 << std::endl;

    return 0;
}
