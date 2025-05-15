#include "query-handler.h"
#include "absl/container/btree_map.h"
#include "constants.h"
#include "git2.h"
#include "query.hpp"
#include <iterator>
#include <stdexcept>
#include <unordered_set>
#include <vector>

QueryHandler::QueryHandler(git_repository *repo)
    : db_(constants::DBPath, Xapian::DB_CREATE_OR_OPEN), termgen_(), qp_(),
      repo_(repo) {
  termgen_.set_database(db_);
  termgen_.set_stemming_strategy(Xapian::TermGenerator::STEM_NONE);
  termgen_.set_max_word_length(constants::MaxWordLen);
  qp_.set_database(db_);
  qp_.set_stemming_strategy(Xapian::QueryParser::STEM_NONE);
}

std::vector<std::vector<std::string>>
QueryHandler::Execute(const Query &query) {
  std::vector<std::vector<std::string>> res;
  if (query.From() == COMMITS) {
    filterCommitsByBTree(query);
    filterCommitsByTextSearch(query);
    for (auto it = commits_.begin(); it != commits_.end(); ++it) {
      std::vector<std::string> fields;
      if (query.Select() & constants::Hash) {
        fields.push_back((*it)->hash);
      }
      if (query.Select() & constants::AuthorName) {
        fields.push_back((*it)->author_name);
      }
      if (query.Select() & constants::AuthorEmail) {
        fields.push_back((*it)->author_email);
      }
      if (query.Select() & constants::Message) {
        fields.push_back((*it)->message);
      }
      if (query.Select() & constants::Date) {
        fields.push_back((*it)->date);
      }
      res.push_back(fields);
    }
  }
  return res;
}

void QueryHandler::filterCommitsByBTree(const Query &query) {
  std::unordered_set<CommitInfo *> s, sn;
  bool first_commit = true;
  auto Apply = [&](const auto &btree, const auto value) {
    auto low = btree.lower_bound(value);
    auto high = btree.upper_bound(value);
    for (auto it = low; it != high; ++it) {
      auto *commit = it->second.get();
      if (first_commit || s.contains(commit)) {
        sn.insert(commit);
      }
    }
  };
  for (auto &clause : query.Where()) {
    sn.clear();
    auto key = clause.Key, value = clause.Value;
    // auto time = query.Where()[i].Time.tm_sec;
    if (key == "author_name") {
      Apply(btree_author_name_, value);
    } else if (key == "author_name") {
      Apply(btree_author_email_, value);
    // } else if (key == "date") {
    //   Apply(btree_date_, value);
    }
    s.swap(sn);
    first_commit = false;
  }
  commits_.resize(s.size());
  std::copy(s.begin(), s.end(), std::back_inserter(commits_));
}

void QueryHandler::filterCommitsByTextSearch(const Query &query) {
  db_.begin_transaction();
  for (const auto &commit : commits_) {
    for (const auto &file : commit->files) {
      git_oid oid;
      if (git_oid_fromstr(&oid, file.data()) != 0) {
        throw std::runtime_error("Error: git_oid_fromstr");
      }
      git_blob *blob;
      if (git_blob_lookup(&blob, repo_, &oid) != 0) {
        throw std::runtime_error("Error: git_blob_lookup");
      }
      const void *content = git_blob_rawcontent(blob);
      size_t size = git_blob_rawsize(blob);
      std::string result(reinterpret_cast<const char *>(content), size);
      Xapian::Document doc;
      termgen_.set_document(doc);
      termgen_.index_text(result);
      doc.set_data(file);
      db_.add_document(doc);
      git_blob_free(blob);
    }
  }
  db_.commit_transaction();
  std::unordered_set<std::string> s, sn;
  bool first_clause = false;
  for (const auto &clause : query.Where()) {
    sn.clear();
    if (clause.Type != WHERE_CLAUSE_TYPE_CONTAINS) {
      continue;
    }
    // add to enum difference between contains in file or in message ?
    Xapian::Query q =
        qp_.parse_query(clause.Value, Xapian::QueryParser::FLAG_PHRASE);
    Xapian::Enquire enq(db_);
    Xapian::MSet mset = enq.get_mset(0, db_.get_doccount());
    std::unordered_set<std::string> files;
    for (auto it = mset.begin(); it != mset.end(); ++it) {
      if (first_clause || s.contains(it.get_document().get_data())) {
        sn.insert(it.get_document().get_data());
      }
    }
    s.swap(sn);
  }
  file_names_.resize(s.size());
  std::copy(s.begin(), s.end(), std::back_inserter(file_names_));
}
