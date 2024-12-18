#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_file = bp::separate[
        "Register A: " >> bp::ulong_long >> bp::eol >>
        "Register B: " >> bp::ulong_long >> bp::eol >>
        "Register C: " >> bp::ulong_long >> bp::eol >>
        bp::eol >>
        "Program: " >> bp::ulong_long % ',' >> bp::eol >> bp::eoi];

    const auto example = std::string{R"EOF(Register A: 729
Register B: 0
Register C: 0

Program: 0,1,5,4,3,0
)EOF"};
    const auto example_2 = std::string{R"EOF(Register A: 10
Register B: 0
Register C: 0

Program: 5,0,5,1,5,4
)EOF"};
    //auto result = bp::parse(example, p_file);
    //auto result = bp::parse(example_2, p_file);
    auto result = bp::parse(file_as_string("2024/17.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    auto& input = *result;

    auto& [a, b, c, program] = input;

    std::cout << a << ", " << b << ", " << c << ", " << program << std::endl;

    //part 1
    const auto run_program = [](auto a, auto b, auto c, const auto& program) -> std::vector<unsigned long long> {
        std::vector<unsigned long long> out;
        for (size_t ip = 0; ip < program.size() - 1;) {
            auto incr = true;
            const auto op = program[ip];
            const auto operand = program[ip + 1];
            const auto combo = [&](const auto x) -> size_t {
                if (x <= 3) {
                    return x;
                } else if (x == 4) {
                    return a;
                } else if (x == 5) {
                    return b;
                } else if (x == 6) {
                    return c;
                }
                ASSERT(false, x);
                return 0;
            };
            if (op == 0) {
                a = a >> combo(operand);
            } else if (op == 1) {
                b = b ^ operand;
            } else if (op == 2) {
                b = combo(operand) & 7;
            } else if (op == 3) {
                if (a != 0) {
                    ip = operand;
                    incr = false;
                }
            } else if (op == 4) {
                b = b ^ c;
            } else if (op == 5) {
                const auto o = combo(operand) & 7;
                out.push_back(o);
            } else if (op == 6) {
                b = a >> combo(operand);
            } else if (op == 7) {
                c = a >> combo(operand);
            } else {
                ASSERT(false, op);
            }
            if (incr) {
                ip += 2;
            }
        }
        return out;
    };
    std::cout << "part 1 = " << run_program(a, b, c, program) << std::endl;;
    std::cout << "registers: a = " << a << ", b = " << b << ", c = " << c << std::endl;


    //part 2
    //combo() function cant be inverted

    //no arithmetic, only bitwise, so bits are independent of each other
    //and the set of possible bit patterns is efficiently stored in 2 uint64_t values

    //e.g. (011, 111) is (x11)
    //(x11) >> 1 is (0x1)
    //(0x1) ^ (111) is (1x0)
    //xxx >> 1 is 0xx

    //(0x1) & (111) is (0x0)
    //(0xx) & (x1x) is (0xx)

    //if xxx >> 1 is 001
    //0xx is 001
    struct bit {
        uint64_t data;
        uint64_t known = ~0ull; //default is all-bits-known to avoid typing ~0ull repeatedly
        bit operator>>(size_t n) const {
            bit tmp = *this;
            tmp.data >>= n;
            tmp.known >>= n;
            //TODO set high bits as known
            return tmp;
        }
        bit operator^(const bit& other) const {
            bit tmp = *this;
            tmp.data ^= other.data;
            tmp.known &= other.known;
            //FIXME check these too
            return tmp;
        }
        bit operator&(const bit& other) const {
            bit tmp = *this;
            tmp.data &= other.data;
            tmp.known |= other.known;
            //FIXME check these too
            return tmp;
        }
        bool compatible(const bit& other) const {
            return (data & known) == (other.data & other.known);
        }
    };
    const auto run_program_all = [](auto a, auto b, auto c, const auto& program, const auto& target_output) -> bit {
        auto i = a;
        for (size_t ip = 0, output_i = 0; ip < program.size() - 1;) {
            auto incr = true;
            const auto op = program[ip];
            const auto operand = program[ip + 1];
            const auto combo = [&](const auto x) -> bit {
                if (x == 0 || x == 1 || x == 2 || x == 3) {
                    return bit{x};
                } else if (x == 4) {
                    return a;
                } else if (x == 5) {
                    return b;
                } else if (x == 6) {
                    return c;
                }
                ASSERT(false);
                return {};
            };
            if (op == 0) {
                a = a >> combo(operand).data;
            } else if (op == 1) {
                b = b ^ bit{operand};
            } else if (op == 2) {
                b = combo(operand) & bit{0b111};
            } else if (op == 3) {
                if (a.data != 0) {
                    ip = operand;
                    incr = false;
                }
            } else if (op == 4) {
                b = b ^ c;
            } else if (op == 5) {
                const auto o = combo(operand) & bit{0b111};
                const auto target_o = bit{target_output[output_i], 0b111};
                output_i++;
                //FIXME
                //provenance _between_ registers is lost
                //we don't know whether these bits apply to register a or any other data source
                //_except_ the only unknown bits come from register a

                //FIXME this crashes
                //bug is probably in bit.known operator overloads
                ASSERT(o.compatible(target_o));
                ASSERT(o.compatible(i));
                //apply known bits back to i
                //FIXME this is probably wrong
                //i = i | o;
            } else if (op == 6) {
                b = a >> combo(operand).data;
            } else if (op == 7) {
                c = a >> combo(operand).data;
            } else {
                ASSERT(false, op);
            }
            if (incr) {
                ip += 2;
            }
        }
        return i;
    };

    {
        const auto a = bit{0, 0};
        const auto b = bit{0};
        const auto c = bit{0};

        const auto a_out = run_program_all(a, b, c, program, program);
    }

    return 0;
}
