#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_line = bp::repeat(5)[bp::char_("#.")] >> bp::eol;
    const auto p_schematic = bp::repeat(7)[p_line];
    const auto p_file = p_schematic % bp::eol >> bp::eoi;

    const auto example = std::string{R"EOF(#####
.####
.####
.####
.#.#.
.#...
.....

#####
##.##
.#.##
...##
...#.
...#.
.....

.....
#....
#....
#...#
#.#.#
#.###
#####

.....
.....
#.#..
###..
###.#
###.#
#####

.....
.....
.....
#....
#.#..
#.#.#
#####
)EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2024/25.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& input = *result;

    std::vector<std::array<int, 5>> keys;
    std::vector<std::array<int, 5>> locks;
    for (const auto& schematic: input) {
        ASSERT(schematic.size() == 7);
        ASSERT(schematic[0].size() == 5);
        std::array<int, 5> pin_heights{};
        if (schematic.front().front() == '#' && schematic.back().front() == '.') {
            //lock
            for (int height = 0; height < 7; height++) {
                for (int pin = 0; pin < 5; pin++) {
                    if (schematic[height][pin] == '#') {
                        pin_heights[pin] = std::max(pin_heights[pin], height);
                    }
                }
            }
            locks.push_back(pin_heights);
            std::cout << "lock: " << pin_heights << std::endl;
        } else if (schematic.front().front() == '.' && schematic.back().front() == '#') {
            //key
            for (int height = 0; height < 7; height++) {
                for (int pin = 0; pin < 5; pin++) {
                    if (schematic[height][pin] == '.') {
                        pin_heights[pin] = std::max(pin_heights[pin], height);
                    }
                }
            }
            for (int pin = 0; pin < 5; pin++) {
                pin_heights[pin] = 5 - pin_heights[pin];
            }
            keys.push_back(pin_heights);
            std::cout << "key : " << pin_heights << std::endl;
        } else {
            ASSERT(false);
        }
    }

    std::ranges::sort(keys);
    std::ranges::sort(locks);

    size_t count = 0;
    for (const auto& lock: locks) {
        for (const auto& key: keys) {
            bool fit = true;
            for (int pin = 0; pin < 5; pin++) {
                if (lock[pin] + key[pin] > 5) {
                    fit = false;
                    break;
                }
            }
            if (fit) {
                count++;
            }
        }
    }

    //part 1
    std::cout << "part 1 = " << count << std::endl;

    //part 2
    std::cout << "part 2 = " << 0 << std::endl;

    return 0;
}
