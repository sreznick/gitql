#include "config.hpp"
#include <cstring>
#include <stdexcept>

Config::Config(int argc, char* argv[]) {
    for (size_t i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-query") == 0) {
            if (i + 1 == argc) {
                throw std::invalid_argument("query flag does not have any value");
            }
            query_ = argv[i + 1];
        } else if (std::strcmp(argv[i], "-help") == 0) {
            help_ = true;
            return;
        }
    }
}

const std::string& Config::Query() const {
    return query_;
}

bool Config::Help() const {
    return help_;
}