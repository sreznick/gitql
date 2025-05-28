#pragma once

#include <vector>
#include <string>
#include "TParser.h"
#include <ctime>
#define SELECT_HASH_MASK (1ull << 0)
#define SELECT_AUTHOR_NAME_MASK (1ull << 1)
#define SELECT_AUTHOR_EMAIL_MASK (1ull << 2)
#define SELECT_MESSAGE_MASK (1ull << 3)
#define SELECT_FILES_MASK (1ull << 4)
#define SELECT_DATE_MASK (1ull << 5)    

enum WhereClauseType {
    WHERE_CLAUSE_TYPE_EQUAL,
    WHERE_CLAUSE_TYPE_CONTAINS,
    WHERE_CLAUSE_TYPE_DAY,
    WHERE_CLAUSE_TYPE_MONTH,
    WHERE_CLAUSE_TYPE_YEAR,
    WHERE_CLAUSE_TYPE_RANGE
};

struct WhereClause {
    enum WhereClauseType Type;
    mutable std::tm FromTime;
    mutable std::tm ToTime;
    std::string Key;
    std::string Value;
    uint64_t FromTimeSeconds() const {
        return timegm(&FromTime);
    };
    uint64_t ToTimeSeconds() const {
        return timegm(&ToTime);
    };
};

enum From {
    COMMITS
};

class Query {
public:
    Query(gitql::TParser::MainContext *ctx);
    uint64_t Select() const;
    enum From From() const;
    const std::vector<WhereClause>& Where() const;
    std::size_t Limit() const;
    std::size_t Offset() const;
private:
    uint64_t select_ = 0;
    enum From from_;
    std::vector<WhereClause> where_;
    std::size_t offset_;
    std::size_t limit_;
};