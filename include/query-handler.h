#pragma once

#include "absl/container/btree_map.h"
#include "commit-info.h"
#include "query.hpp"
#include "xapian.h"
#include <chrono>
#include <git2/types.h>
#include <unordered_set>
class QueryHandler {
public:
  QueryHandler(git_repository *repo);
  std::vector<std::vector<std::string>> Execute(const Query &query);

private:
  void filterCommitsByBTree(const Query &query);
  void filterCommitsByTextSearch(const Query &query);

  Xapian::WritableDatabase db_;
  
  std::vector<CommitInfo *> commits_;
  std::vector<std::string> file_names_;
  
  absl::btree_map<std::string, std::unique_ptr<CommitInfo>> btree_author_name_;
  absl::btree_map<std::string, std::unique_ptr<CommitInfo>> btree_author_email_;
  absl::btree_map<std::chrono::seconds, std::unique_ptr<CommitInfo>>
      btree_date_;

  Xapian::TermGenerator termgen_;
  Xapian::QueryParser qp_;
  
  git_repository *repo_;
};
