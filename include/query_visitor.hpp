#pragma once
#include "TParserBaseVisitor.h"
#include "TParser.h"
#include "TParserVisitor.h"
#include "query.hpp"
#include <vector>

#define DEFAULT_LIMIT 10
#define DEFAULT_OFFSET 0

class QueryVisitor : public gitql::TParserBaseVisitor {
public:
    std::any visitEqual(gitql::TParser::EqualContext* ctx) override;
    std::any visitContains(gitql::TParser::ContainsContext* ctx) override;
    std::any visitFields(gitql::TParser::FieldsContext* ctx) override;
    std::any visitLimitFirst(gitql::TParser::LimitFirstContext* ctx) override;
    std::any visitOffsetFirst(gitql::TParser::OffsetFirstContext* ctx) override;
    const std::vector<WhereClause>& Where() const;
    uint64_t Select() const;
    std::size_t Limit() const;
    std::size_t Offset() const;
private:
    std::vector<WhereClause> where_;
    uint64_t select_ = 0;
    std::size_t limit_;
    std::size_t offset_;
};