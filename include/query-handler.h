#pragma once

#include "absl/container/btree_map.h"
#include "commit-info.h"
#include "query.hpp"
#include "xapian.h"
#include <absl/container/flat_hash_map.h>
#include <chrono>
#include <git2/types.h>
#include <memory>
#include <unordered_set>
#include <vector>
class QueryHandler {
public:
  QueryHandler(git_repository *repo,
               std::vector<std::unique_ptr<CommitInfo>> &commits);
  std::vector<std::vector<std::string>> Execute(const Query &query);

private:
  void filterCommitsByBTree(const Query &query);
  void filterCommitsByTextSearch(const Query &query);
  bool hasContainsClause(const Query &query);
  Xapian::WritableDatabase db_;

  std::vector<CommitInfo *> commits_;
  std::vector<std::string> file_names_;

  absl::btree_map<std::string, std::vector<CommitInfo *>> btree_author_name_;
  absl::btree_map<std::string, std::vector<CommitInfo *>> btree_author_email_;
  absl::btree_map<std::chrono::seconds, CommitInfo *> btree_date_;

  absl::flat_hash_map<std::string, std::vector<CommitInfo *>> file_to_commits_;
  Xapian::TermGenerator termgen_;
  Xapian::QueryParser qp_;

  git_repository *repo_;
};
