#include "query.hpp"
#include "query_visitor.hpp"

Query::Query(gitql::TParser::MainContext *ctx) {
    QueryVisitor visitor;
    if (ctx->portion_clause()) {
        ctx->portion_clause()->accept(&visitor);
        limit_ = visitor.Limit();
        offset_ = visitor.Offset();
    }
    from_ = COMMITS;
    if (ctx->where_clause()) {
        ctx->where_clause()->accept(&visitor);
    }
    where_ = visitor.Where();
    ctx->fields()->accept(&visitor);
    select_ = visitor.Select();
}

uint64_t Query::Select() const {
    return select_;
}

enum From Query::From() const {
    return from_;
}

const std::vector<WhereClause>& Query::Where() const {
    return where_;
}

std::size_t Query::Offset() const {
    return offset_;
}

std::size_t Query::Limit() const {
    return limit_;
}