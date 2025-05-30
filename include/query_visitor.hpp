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
    antlrcpp::Any visitEqual(gitql::TParser::EqualContext* ctx) override;
    antlrcpp::Any visitRange(gitql::TParser::RangeContext* ctx) override;
    antlrcpp::Any visitContains(gitql::TParser::ContainsContext* ctx) override;
    antlrcpp::Any visitFields(gitql::TParser::FieldsContext* ctx) override;
    antlrcpp::Any visitLimitFirst(gitql::TParser::LimitFirstContext* ctx) override;
    antlrcpp::Any visitOffsetFirst(gitql::TParser::OffsetFirstContext* ctx) override;
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