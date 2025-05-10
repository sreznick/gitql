#pragma once
#include "TParserBaseVisitor.h"
#include "TParser.h"
#include "query.hpp"
#include <vector>

#define DEFAULT_LIMIT 10
#define DEFAULT_OFFSET 0

class QueryVisitor : public gitql::TParserBaseVisitor {
public:
    std::any visitCondition(gitql::TParser::ConditionContext* ctx) override;
    std::any visitFields(gitql::TParser::FieldsContext* ctx) override;
    std::any visitPortion_clause(gitql::TParser::Portion_clauseContext* ctx) override;
    const std::vector<WhereClause>& Where() const;
    const std::vector<std::string>& Select() const;
    std::size_t Limit() const;
    std::size_t Offset() const;
private:
    std::vector<WhereClause> where_;
    std::vector<std::string> select_;
    std::size_t limit_;
    std::size_t offset_;
};