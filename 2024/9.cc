#include "util.hh"

std::ostream& operator<<(std::ostream& os, const std::vector<std::pair<int, int>>& disk_map) {
    for (size_t i = 0; i < disk_map.size(); i++) {
        const auto& [used, free] = disk_map[i];
        for (int j = 0; j < used; j++) {
            std::cout << i;
        }
        for (int j = 0; j < free; j++) {
            std::cout << '.';
        }
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::pair<size_t, uint8_t>>& disk_map) {
    for (size_t i = 0; i < disk_map.size(); i++) {
        const auto& [id, used] = disk_map[i];
        for (int j = 0; j < used; j++) {
            std::cout << id;
        }
    }
    return os;
}

struct block_t {
    size_t id;
    size_t used;
    size_t free;
};
std::ostream& operator<<(std::ostream& os, const std::list<block_t>& block_list) {
    for (const auto& block: block_list) {
        for (size_t i = 0; i < block.used; i++) {
            os << block.id;
        }
        for (size_t i = 0; i < block.free; i++) {
            os << ".";
        }
    }
    return os;
}

auto main() -> int {
    namespace bp = boost::parser;

    const auto p_file = *bp::digit >> bp::eol;

    const auto example = std::string{R"EOF(2333133121414131402
)EOF"};
    const auto file = file_as_string("2024/9.input.txt");

    using namespace util;

    //part 1
    for (const auto& input: {example}) {
        auto result = bp::parse(input, p_file);
        if (!result) {
            std::cerr << "failed to parse input" << std::endl;
            return 1;
        }

        const std::string& disk = *result;
        const auto flatten_disk = [](const std::string& disk) {
            std::vector<std::optional<uint64_t>> out;
            for (size_t i = 0; i < disk.size(); i++) {
                const auto used = ((i % 2) == 0);
                const size_t size = disk[i] - '0';
                const uint64_t id = i / 2;
                for (size_t j = 0; j < size; j++) {
                    out.push_back(used ? std::optional{id} : std::nullopt);
                }
            }
            return out;
        };
        auto disk_flat = flatten_disk(disk);
        for (size_t i = 0, j = disk_flat.size() - 1; i < j;) {
            auto& a = disk_flat[i];
            auto& b = disk_flat[j];
            if (a == std::nullopt && b != std::nullopt) {
                a = b;
                b = std::nullopt;
            }
            if (a != std::nullopt) {
                i++;
            }
            if (b == std::nullopt) {
                j--;
            }
        }
        ASSERT(std::ranges::is_partitioned(disk_flat, [](const auto& c) -> bool { return c != std::nullopt; }));

        const auto checksum = [](const auto& disk) {
            size_t checksum = 0;
            for (size_t i = 0; i < disk.size(); i++) {
                if (disk[i] == std::nullopt) {
                    break;
                }
                const auto id = *disk[i];
                checksum += i * id;
            }
            return checksum;
        };
        std::cout << "p1 checksum = " << checksum(disk_flat) << std::endl;
    }

    //part 1 - method 2 (flatten)
    for (const auto& input: {example}) {
        auto result = bp::parse(input, p_file);
        if (!result) {
            std::cerr << "failed to parse input" << std::endl;
            return 1;
        }

        std::string& disk = *result;

        const auto flatten_disk = [](const std::string& disk) {
            std::vector<std::optional<uint64_t>> out;
            for (size_t i = 0; i < disk.size(); i++) {
                const auto used = ((i % 2) == 0);
                const size_t size = disk[i] - '0';
                const uint64_t id = i / 2;
                for (size_t j = 0; j < size; j++) {
                    out.push_back(used ? std::optional{id} : std::nullopt);
                }
            }
            return out;
        };
        auto disk_flat = flatten_disk(disk);
        for (size_t i = 0, j = disk_flat.size() - 1; i < j;) {
            auto& a = disk_flat[i];
            auto& b = disk_flat[j];
            if (a == std::nullopt && b != std::nullopt) {
                a = b;
                b = std::nullopt;
            }
            if (a != std::nullopt) {
                i++;
            }
            if (b == std::nullopt) {
                j--;
            }
        }
        ASSERT(std::ranges::is_partitioned(disk_flat, [](const auto& c) -> bool { return c != std::nullopt; }));

        const auto checksum = [](const auto& disk) {
            size_t checksum = 0;
            for (size_t i = 0; i < disk.size(); i++) {
                if (disk[i] == std::nullopt) {
                    break;
                }
                const auto id = *disk[i];
                checksum += i * id;
            }
            return checksum;
        };
        std::cout << "p1 checksum = " << checksum(disk_flat) << std::endl;
    }

    //part 2
    for (const auto& input: {example, file}) {
        auto result = bp::parse(input, p_file);
        if (!result) {
            std::cerr << "failed to parse input" << std::endl;
            return 1;
        }

        std::string& disk = *result;
        if ((disk.size() % 2) == 1) {
            disk.push_back('0');
        }

        std::list<block_t> block_list;
        for (size_t i = 0; i + 1 < disk.size(); i += 2) {
            block_list.push_back(block_t{
                .id = i / 2,
                .used = static_cast<size_t>(disk[i] - '0'),
                .free = static_cast<size_t>(disk[i + 1] - '0'),
            });
        }

        for (auto it = --block_list.end(); it != block_list.begin();) {
            auto block = *it;
            const auto used = block.used;
            auto new_it = std::find_if(block_list.begin(), it, [used](const auto& block) {
                return block.free >= used;
            });
            if (new_it == it) {
                --it;
                continue;
            }
            auto new_next = std::next(new_it);
            auto old_prev = std::prev(it);

            old_prev->free += block.used + block.free;
            block.free = new_it->free - block.used;
            new_it->free = 0;

            block_list.insert(new_next, block);
            auto tmp = it;
            --it;
            block_list.erase(tmp);
        }

        size_t checksum = 0;
        size_t pos = 0;
        for (const auto& block: block_list) {
            const auto end = pos + block.used;
            for (; pos < end; pos++) {
                checksum += block.id * pos;
            }
            pos += block.free;
        }
        std::cout << "p2 checksum = " << checksum << std::endl;
    }
    return 0;
}
