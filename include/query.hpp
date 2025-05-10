#pragma once

#include <vector>
#include <string>
#include "TParser.h"

struct WhereClause {
    std::string Key;
    std::string Value;
};

class Query {
public:
    Query(gitql::TParser::MainContext *ctx);
    const std::vector<std::string>& Select() const;
    const std::string& From() const;
    const std::vector<WhereClause>& Where() const;
    std::size_t Limit() const;
    std::size_t Offset() const;
private:
    std::vector<std::string> select_;
    std::string from_;
    std::vector<WhereClause> where_;
    std::size_t offset_;
    std::size_t limit_;
};