#include "query_visitor.hpp"

std::any QueryVisitor::visitCondition(gitql::TParser::ConditionContext* ctx) {
    std::string key = ctx->FIELD()->getText();
    std::string value = ctx->QUOTE()->getText();
    value = value.substr(1, value.size() - 2);
    where_.push_back({key, value});
    return nullptr;
}

const std::vector<WhereClause>& QueryVisitor::Where() const {
    return where_;
}

std::any QueryVisitor::visitFields(gitql::TParser::FieldsContext* ctx) {
    if (ctx->ALL()) {
        select_.push_back("*");
    } else {
        for (const auto& field: ctx->FIELD()) {
            select_.push_back(field->getText());
        }
    }
    return nullptr;
}

const std::vector<std::string>& QueryVisitor::Select() const {
    return select_;
}

std::any QueryVisitor::visitPortion_clause(gitql::TParser::Portion_clauseContext* ctx) {
    std::size_t portionIndex = 0;
    if (ctx->LIMIT()) {
        limit_ = std::stoul(ctx->NUM().at(portionIndex++)->getText());
    } else {
        limit_ = DEFAULT_LIMIT;
    }
    if (ctx->OFFSET()) {
        offset_ = std::stoul(ctx->NUM().at(portionIndex)->getText());
    } else {
        offset_ = DEFAULT_OFFSET;
    }
    return nullptr;
}

std::size_t QueryVisitor::Limit() const {
    return limit_;
}

std::size_t QueryVisitor::Offset() const {
    return offset_;
}