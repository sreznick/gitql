#pragma once

#include <string>

class Config {
public:
    Config(int argc, char* argv[]);
    const std::string& Query() const;
    bool Help() const;
private:
    std::string query_;
    bool help_ = true;
};