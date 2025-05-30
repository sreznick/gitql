#include "query-handler.h"
#include "absl/container/btree_map.h"
#include "commit-info.h"
#include "commits-proc.h"
#include "constants.h"
#include "git2.h"
#include "query.hpp"
#include <chrono>
#include <iterator>
#include <stdexcept>
#include <unordered_set>
#include <vector>

QueryHandler::QueryHandler(git_repository *repo,
                           std::vector<std::unique_ptr<CommitInfo>> &commits)
    : db_(constants::DBPath, Xapian::DB_CREATE_OR_OPEN), termgen_(), qp_(),
      repo_(repo) {
  termgen_.set_database(db_);
  termgen_.set_stemming_strategy(Xapian::TermGenerator::STEM_NONE);
  termgen_.set_max_word_length(constants::MaxWordLen);
  qp_.set_database(db_);
  qp_.set_stemming_strategy(Xapian::QueryParser::STEM_NONE);
  for (const auto &commit : commits) {
    auto *ci = commit.get();
    for (auto &file : ci->files) {
      file_to_commits_[file].push_back(ci);
    }
    commits_.push_back(ci);
    btree_author_name_[ci->author_name].push_back(ci);
    btree_author_email_[ci->author_email].push_back(ci);
    btree_date_[ci->date].push_back(ci);
  }
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
        fields.push_back(std::to_string((*it)->date.count()));
      }
      if (query.Select() & constants::Files) {
        fields.insert(fields.end(), (*it)->files.begin(), (*it)->files.end());
      }
      res.push_back(fields);
    }
  }
  return res;
}

void QueryHandler::filterCommitsByBTree(const Query &query) {
  std::unordered_set<CommitInfo *> s, sn;
  bool first_commit = true;
  auto Apply = [&](const auto &btree, const auto left, const auto right) {
    auto low = btree.lower_bound(left);
    auto high = btree.upper_bound(right);
    for (auto it = low; it != high; ++it) {
      auto commits = it->second;
      for (const auto &commit : commits) {
        if (first_commit || s.count(commit)) {
          sn.insert(commit);
        }
      }
    }
  };
  for (auto &clause : query.Where()) {
    sn.clear();
    auto key = clause.Key, value = clause.Value;
    if (key == "author_name") {
      Apply(btree_author_name_, value, value);
    } else if (key == "author_email") {
      Apply(btree_author_email_, value, value);
    } else if (key == "date") {
      Apply(btree_date_, std::chrono::seconds(clause.FromTimeSeconds()),
            std::chrono::seconds(clause.ToTimeSeconds()));
    } else if (clause.Type == WHERE_CLAUSE_TYPE_DAY ||
               clause.Type == WHERE_CLAUSE_TYPE_MONTH ||
               clause.Type == WHERE_CLAUSE_TYPE_YEAR) {
      Apply(btree_date_, std::chrono::seconds(clause.FromTimeSeconds()),
            std::chrono::seconds(clause.ToTimeSeconds()));
    } else {
      continue;
    }
    s.swap(sn);
    first_commit = false;
  }
  if (!first_commit) {
    commits_.assign(s.begin(), s.end());
  }
}

bool QueryHandler::hasContainsClause(const Query &query) {
  for (const auto &clause : query.Where()) {
    if (clause.Type == WHERE_CLAUSE_TYPE_CONTAINS) {
      return true;
    }
  }
  return false;
}

void QueryHandler::filterCommitsByTextSearch(const Query &query) {
  if (!hasContainsClause(query)) {
    return;
  }
  db_.begin_transaction();
  for (const auto &commit : commits_) {
    git_commit *c = GetCommitFromHash(commit->hash, repo_);
    auto files = GetCommitFiles(c, repo_);
    git_commit_free(c);
    commit->files = files;
    for (const auto &file : commit->files) {
      file_to_commits_[file].push_back(commit);
      git_oid oid;
      if (git_oid_fromstr(&oid, file.data()) != 0) {
        throw std::runtime_error("filterCommitsByTextSearch: git_oid_fromstr");
      }
      git_blob *blob;
      if (git_blob_lookup(&blob, repo_, &oid) != 0) {
        throw std::runtime_error("filterCommitsByTextSearch: git_blob_lookup");
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
  bool first_clause = true;
  for (const auto &clause : query.Where()) {
    sn.clear();
    if (clause.Type != WHERE_CLAUSE_TYPE_CONTAINS) {
      continue;
    }
    Xapian::Query q =
        qp_.parse_query(clause.Value, Xapian::QueryParser::FLAG_PHRASE);
    Xapian::Enquire enq(db_);
    enq.set_query(q);
    Xapian::MSet mset = enq.get_mset(0, db_.get_doccount());
    std::unordered_set<std::string> files;
    for (auto it = mset.begin(); it != mset.end(); ++it) {
      if (first_clause || s.count(it.get_document().get_data())) {
        sn.insert(it.get_document().get_data());
      }
    }
    s.swap(sn);
    first_clause = false;
  }
  file_names_.assign(s.begin(), s.end());
  commits_.clear();
  std::unordered_set<CommitInfo *> commits_set;
  for (const auto &file : file_names_) {
    auto commits = file_to_commits_[file];
    commits_set.insert(commits.begin(), commits.end());
  }
  commits_.assign(commits_set.begin(), commits_set.end());
}
