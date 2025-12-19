#include "util.hh"

auto main(int argc, const char* argv[]) -> int {
    namespace bp = boost::parser;

    const auto p_line =
        '[' >> *bp::char_(".#") >> ']' >> bp::blank >>
        bp::separate[
            ('(' >> bp::ulong_ % ',' >> ')') % bp::blank >> bp::blank >>
            '{' >> bp::ulong_ % ',' >> '}'
        ] >> bp::eol;
    bp::ulong_long % bp::blank >> bp::eol;
    const auto p_file = *p_line >> bp::eoi;

    const auto example = std::string{R"EOF([.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
[...#.] (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}
[.###.#] (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}
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
#if 0
    size_t sum = 0;
    for (const auto& line: input) {
        const auto& [target_lights, buttons, target_volts] = line;
        std::deque<std::tuple<size_t, std::string, size_t>> next_state;
        for (size_t i = 0; i < buttons.size(); i++) {
            next_state.push_back({1, std::string(target_lights.size(), '.'), i});
        }
        while (true) {
            auto [presses, current_volts, next_button] = next_state.front();
            next_state.pop_front();
            for (const auto button_index: buttons[next_button]) {
                auto& c = current_volts[button_index];
                if (c == '.') {
                    c = '#';
                } else {
                    c = '.';
                }
            }
            if (current_volts == target_lights) {
                std::cout << std::format("found {} {} {}\n", presses, target_lights, current_volts);
                sum += presses;
                break;
            }
            for (size_t i = 0; i < buttons.size(); i++) {
                next_state.push_back({presses + 1, current_volts, i});
            }
        }
    }
    std::cout << "part 1 = " << sum << std::endl;
#endif

    //part 2
    size_t sum = 0;
    for (const auto& line: input) {
        const auto& [_, buttons, target_volts] = line;
        for (size_t i = 0; i < target_volts.size(); i++) {
            ASSERT(target_volts[i] <= 255);
        }
        size_t max_presses = std::ranges::max(target_volts);
        const auto distance = [target_volts](const auto& a){
            const auto& [_, current_volts] = a;
            size_t sum = 0;
            for (size_t i = 0; i < target_volts.size(); i++) {
                sum += target_volts[i] - current_volts[i];
            }
            return sum;
        };
        const auto compare = [distance](const auto& a, const auto& b){
            return distance(a) < distance(b) || (distance(a) == distance(b) && std::get<0>(a) < std::get<0>(b));
        };
        std::priority_queue<
            std::tuple<size_t, std::vector<uint8_t>>,
            std::vector<std::tuple<size_t, std::vector<uint8_t>>>,
            decltype(compare)
        > next_state(compare);
        next_state.push({0, std::vector<uint8_t>(target_volts.size(), 0)});
        while (true) {
            const auto [presses, prev_volts] = next_state.top();
            next_state.pop();
            for (size_t next_button = 0; next_button < buttons.size(); next_button++) {
                auto current_volts = prev_volts;
                bool broken = false;
                for (const auto button_index: buttons[next_button]) {
                    current_volts[button_index] += 1;
                    if (current_volts[button_index] > target_volts[button_index]) {
                        broken = true;
                        break;
                    }
                }
                if (broken) {
                    continue;
                }
                bool all = true;
                for (size_t i = 0; i < current_volts.size(); i++) {
                    if (current_volts[i] != target_volts[i]) {
                        all = false;
                        break;
                    }
                }
                if (all) {
                    std::cout << std::format("found {} {}\n", presses, current_volts);
                    sum += presses + 1;
                    break;
                }
                if (presses + 1 > max_presses) {
                    continue;
                }
                next_state.push({presses + 1, current_volts});
            }
        }
    }
    std::cout << "part 2 = " << sum << std::endl;

    return 0;
}
