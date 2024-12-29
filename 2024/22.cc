#include "util.hh"

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_line = bp::ulong_long >> bp::eol;
    const auto p_file = +p_line >> bp::eoi;

    const auto example = std::string{R"EOF()EOF"};
    //const auto result = bp::parse(example, p_file);
    const auto result = bp::parse(file_as_string("2024/22.input.txt"), p_file);
    if (!result) {
        std::cerr << "failed to parse input" << std::endl;
        return 1;
    }

    using namespace util;

    const auto& input = *result;

    const auto secret_evolve = [](size_t s){
        s = ((s * 64) ^ s) % 16777216;
        s = ((s / 32) ^ s) % 16777216;
        s = ((s * 2048) ^ s) % 16777216;
        return s;
    };

    const auto secret_evolve_n = [&](size_t s, const size_t n){
        for (size_t i = 0; i < n; i++) {
            s = secret_evolve(s);
        }
        return s;
    };

    ASSERT(secret_evolve(123) == 15887950);
    ASSERT(secret_evolve(15887950) == 16495136);
    ASSERT(secret_evolve(16495136) == 527345);

    ASSERT(secret_evolve_n(1, 2000) == 8685429);
    ASSERT(secret_evolve_n(10, 2000) == 4700978);
    ASSERT(secret_evolve_n(100, 2000) == 15273692);
    ASSERT(secret_evolve_n(2024, 2000) == 8667524);

    size_t total = 0;
    for (const auto& s: input) {
        total += secret_evolve_n(s, 2000);
    }

    //part 1
    std::cout << "part 1 = " << total << std::endl;

    std::vector<std::vector<size_t>> all_secrets;
    for (auto s: input) {
        all_secrets.push_back({});
        auto& secrets = all_secrets.back();
        secrets.push_back(s);
        for (size_t i = 1; i < 2000; i++) {
            secrets.push_back(secret_evolve(secrets.back()));
            if (i < 4) {
                continue;
            }
        }
    }
    std::vector<std::vector<int>> all_diffs;
    for (const auto& secrets: all_secrets) {
        all_diffs.push_back(std::ranges::to<std::vector<int>>(
            secrets |
            std::views::adjacent<2> |
            std::views::transform([](const auto &p) {
                const auto& [a, b] = p;
                return (b % 10) - (a % 10);
            })
        ));
        ASSERT(all_diffs.back().size() == secrets.size() - 1);
    }

    using sequence_type = std::tuple<int, int, int, int>;
    std::unordered_map<sequence_type, size_t> sequence_prices;
    for (const auto& [secrets, diffs]: std::views::zip(all_secrets, all_diffs)) {
        std::unordered_set<sequence_type> visited;
        for (const auto& [secret, sequence]: std::views::zip(
            secrets | std::views::drop(4),
            diffs | std::views::adjacent<4>)
        ) {
            if (visited.contains(sequence)) {
                continue;
            }
            visited.insert(sequence);
            const auto price = secret % 10;
            sequence_prices[sequence] += price;
        }
    }

    size_t max_price = std::ranges::max(sequence_prices | std::views::values);

    //part 2
    std::cout << "part 2 = " << max_price << std::endl;

    return 0;
}
