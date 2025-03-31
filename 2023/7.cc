#include "util.hh"

template<size_t N, typename T>
auto vec_to_array(const std::vector<T>& v) {
    std::array<T, N> a{};
    ASSERT(v.size() <= a.size());
    std::ranges::move(v, a.begin());
    return a;
};

auto main() -> int {
    namespace bp = boost::parser;

    const bp::symbols<size_t> p_card = {
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
    auto to_hand_type = [](const std::map<size_t, size_t>& hand) {
        const auto top = hand.begin()->second;
        if (top == 5) {
            return hand_type::five;
        } else if (top == 4) {
            return hand_type::four;
        } else if (top == 3 && std::next(hand.begin())->second == 2) {
            return hand_type::three_two;
        } else if (top == 2 && std::next(hand.begin())->second == 2) {
            return hand_type::two_two;
        } else if (top == 2) {
            return hand_type::two;
        } else {
            return hand_type::one;
        }
    };
    auto a_hand_transform = [&](const std::vector<size_t>& v) {
        std::map<size_t, size_t> m;
        for (auto i: v) {
            m[i]++;
        }
        auto x = v;
        return std::make_pair(to_hand_type(m), vec_to_array<5uz>(x));
    };
    const auto p_hand = bp::transform(tuple_to_pair)[
        bp::transform(a_hand_transform)[*p_card] >> bp::blank >> bp::ulong_] >> bp::eol;
    const auto file = *p_hand >> bp::eoi;

    std::string example = R"EOF(32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483
)EOF";

    auto result = bp::parse(example, file);
    //auto result = bp::parse(file_as_string("2023/7.input.txt"), file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    std::vector<std::pair<std::pair<hand_type, std::array<size_t, 5>>, long unsigned>>& hand_bids = *result;

    const auto compare_hands = [](
        const std::pair<std::pair<hand_type, std::array<size_t, 5>>, size_t>& a,
        const std::pair<std::pair<hand_type, std::array<size_t, 5>>, size_t>& b
    ) {
        if (a.first.first > b.first.first) {
            return true;
        } else if (a.first.first < b.first.first) {
            return false;
        } else {
            return a.first.second > b.first.second;
        }
    };

    std::sort(hand_bids.begin(), hand_bids.end(), compare_hands);

    for (const auto& [hand_type_list, bid]: hand_bids) {
        const auto& [t, hand] = hand_type_list;
        for (const auto& c: hand) {
            std::cout << c << ", ";
        }
        std::cout << std::endl;
    }

    size_t sum = 0;
    for (const auto& [index, hand_bid]: hand_bids | std::views::enumerate) {
        const auto& [_, bid] = hand_bid;
        const auto rank = hand_bids.size() - index;
        sum += rank * bid;
        std::cout << rank << ", " << bid << std::endl;
    }
    std::cout << "expects:\n5, 483\n4, 684\n3, 28\n2, 220\n1, 765\n";

    //expect:
    //32T3K
    //KK677
    //KTJJT
    //QQQJA
    //T55J5

    std::cout << "part 1 = " << sum << std::endl;
    std::cout << "part 2 = " << 0 << std::endl;

    return 0;
}
