#include <fstream>
#include <string>

auto file_as_string(std::string filename) -> std::string {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

auto get_or(const auto& container, const auto& key, const auto& default_) -> decltype(default_) {
    if (container.find(key) == container.end()) {
        return default_;
    } else {
        return container.find(key)->second;
    }
}

int sign(auto x) {
    if (x > 0) {
        return 1;
    } else if (x < 0) {
        return -1;
    } else {
        return 0;
    }
}

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
