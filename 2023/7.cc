#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const bp::symbols<int> p_card = {
        {"A", 14},
        {"K", 13},
        {"Q", 12},
        {"J", 11},
        {"T", 10},
        {"9", 9},
        {"8", 8},
        {"7", 7},
        {"6", 6},
        {"5", 5},
        {"4", 4},
        {"3", 3},
        {"2", 2},
    };
    enum class hand_type {
        one,       // high card
        two,       // one pair
        two_two,   // two pair
        three_two, // house
        four,
        five,
    };
    auto to_hand_type = [](std::map<int, int>& hand) {
        auto top = hand.begin()->second;
        auto second = std::next(hand.begin())->second;
        if (top == 5) {
            return hand_type::five;
        } else if (top == 4) {
            return hand_type::four;
        } else if (top == 3 && second == 2) {
            return hand_type::three_two;
        } else if (top == 2 && second == 2) {
            return hand_type::two_two;
        } else if (top == 2) {
            return hand_type::two;
        } else {
            return hand_type::one;
        }
    };
    auto a_hand_transform = [&](std::vector<int> v) {
        std::map<int, int> m;
        for (auto i: v) {
            m[i]++;
        }
        auto x = v;
        return std::make_pair(to_hand_type(m), x);
    };
    const auto p_hand = bp::transform(a_hand_transform)[*p_card] >> bp::blank >> bp::ulong_;
    const auto file = (p_hand % bp::eol) >> *bp::ws;

    std::println("here");
    const auto result = bp::parse(file_as_string("2023/7.input.txt"), file, bp::trace::on);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

#if 0
    std::vector x{0, 1, 2, 3};
    int y = 10;
    std::println("{}", y);

    const std::vector<std::pair<std::pair<hand_type, std::vector<int>>, long unsigned>>& hand_bids = *result;

    const auto compare_hands = [](const auto& a, const auto& b) {
        if (a.first > b.first) {
            return true;
        } else if (a.first < b.first) {
            return false;
        } else {
            return a.second > b.second;
        }
    };

    compare_hands(hand_bids[0], hand_bids[1]);

    //std::sort(hand_bids.begin(), hand_bids.end(), compare_hands);
    std::cout << "part 1 = " << 0 << std::endl;
    std::cout << "part 2 = " << 0 << std::endl;
    #endif

    return 0;
}
