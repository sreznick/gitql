#include "query_visitor.hpp"
#include <iomanip>
#include <sstream>
#include <stdexcept>

antlrcpp::Any QueryVisitor::visitContains(gitql::TParser::ContainsContext* ctx) {
    std::string field = ctx->FIELD()->getText();
    std::string text = ctx->QUOTE()->getText();

    WhereClause where{};
    where.Type = WHERE_CLAUSE_TYPE_CONTAINS;
    where.Key = field;
    where.Value = text;
    where_.push_back(where);
    return nullptr;
}

antlrcpp::Any QueryVisitor::visitRange(gitql::TParser::RangeContext* ctx) {
    std::string field = ctx->FIELD()->getText();
    std::string from = ctx->QUOTE().at(0)->getText();
    from = from.substr(1, from.size() - 2);
    std::string to = ctx->QUOTE().at(1)->getText();
    to = to.substr(1, to.size() - 2);

    WhereClause where{};
    where.Key = field;
    where.Type = WHERE_CLAUSE_TYPE_RANGE;
    std::tm fromTime{};
    std::tm toTime{};
    std::istringstream issFrom{from};
    issFrom >> std::get_time(&fromTime, "%d.%m.%y");
    if (issFrom.fail()) {
        throw std::invalid_argument("from time must be in format: dd.mm.yy");
    }
    where.FromTime = fromTime;
    std::istringstream issTo{to};
    issTo >> std::get_time(&toTime, "%d.%m.%y");
    if (issTo.fail()) {
        throw std::invalid_argument("to time must be in format: dd.mm.yy");
    }
    where.ToTime = toTime;
    where_.push_back(where);
    return nullptr;
}

antlrcpp::Any QueryVisitor::visitEqual(gitql::TParser::EqualContext* ctx) {
    std::string key = ctx->FIELD()->getText();
    std::string value = ctx->QUOTE()->getText();
    value = value.substr(1, value.size() - 2);

    WhereClause where{};
    if (key == "day") {
        where.Type = WHERE_CLAUSE_TYPE_DAY;
        std::tm time{};
        std::istringstream iss{value};
        iss >> std::get_time(&time, "%d.%m.%y");
        if (iss.fail()) {
            throw std::invalid_argument("day value must be in format: dd.mm.yy");
        }
        where.FromTime = time;
        where.ToTime = time;
        where.ToTime.tm_mday += 1;
        std::mktime(&where.ToTime);
    } else if (key == "month") {
        where.Type = WHERE_CLAUSE_TYPE_MONTH;
        std::tm time{};
        std::istringstream iss{value};
        iss >> std::get_time(&time, "%m.%y");
        if (iss.fail()) {
            throw std::invalid_argument("month value must be in format: mm.yy");
        }
        where.FromTime = time;
        where.ToTime = time;
        where.ToTime.tm_mon += 1;
        std::mktime(&where.ToTime);
    } else if (key == "year") {
        where.Type = WHERE_CLAUSE_TYPE_YEAR;
        std::tm time{};
        std::istringstream iss{value};
        iss >> std::get_time(&time, "%y");
        if (iss.fail()) {
            throw std::invalid_argument("year value must be in format: yy");
        }
        where.FromTime = time;
        where.ToTime = time;
        where.ToTime.tm_year += 1;
        std::mktime(&where.ToTime);
    } else {
        where.Type = WHERE_CLAUSE_TYPE_EQUAL;
        where.Key = key;
        where.Value = value;
    }

    where_.push_back(where);
    return nullptr;
}

const std::vector<WhereClause>& QueryVisitor::Where() const {
    return where_;
}

antlrcpp::Any QueryVisitor::visitFields(gitql::TParser::FieldsContext* ctx) {
    if (ctx->ALL()) {
        select_ = ~0ull;
    } else {
        for (const auto& field: ctx->FIELD()) {
            if (field->getText() == "hash") {
                select_ |= SELECT_HASH_MASK;
            } else if (field->getText() == "author_name") {
                select_ |= SELECT_AUTHOR_NAME_MASK;
            } else if (field->getText() == "author_email") {
                select_ |= SELECT_AUTHOR_EMAIL_MASK;
            } else if (field->getText() == "message") {
                select_ |= SELECT_MESSAGE_MASK;
            } else if (field->getText() == "files") {
                select_ |= SELECT_FILES_MASK;
            } else if (field->getText() == "date") {
                select_ |= SELECT_DATE_MASK;
            } else {
                auto err = "invalid select field: " + field->getText();
                throw std::invalid_argument(err);
            }
        }
    }
    return nullptr;
}

uint64_t QueryVisitor::Select() const {
    return select_;
}

antlrcpp::Any QueryVisitor::visitLimitFirst(gitql::TParser::LimitFirstContext* ctx) {
    limit_ = std::stoul(ctx->NUM().at(0)->getText());
    if (ctx->OFFSET()) {
        offset_ = std::stoul(ctx->NUM().at(1)->getText());
    } else {
        offset_ = DEFAULT_OFFSET;
    }
    return nullptr;
}

antlrcpp::Any QueryVisitor::visitOffsetFirst(gitql::TParser::OffsetFirstContext* ctx) {
    offset_ = std::stoul(ctx->NUM().at(0)->getText());
    if (ctx->LIMIT()) {
        limit_ = std::stoul(ctx->NUM().at(1)->getText());
    } else {
        limit_ = DEFAULT_LIMIT;
    }
    return nullptr;
}

std::size_t QueryVisitor::Limit() const {
    return limit_;
}

std::size_t QueryVisitor::Offset() const {
    return offset_;
}