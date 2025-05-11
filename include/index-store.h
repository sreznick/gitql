#pragma once

#include "absl/container/btree_map.h"
#include "commit-info.h"
#include "query.hpp"
#include <unordered_set>

class IndexStore {
public:
  IndexStore() = default;
  std::vector<std::vector<std::string>> Execute(const Query &query);

private:
  void filterCommits(const Query &query);
  std::unordered_set<CommitInfo *> commits_;
  absl::btree_map<std::string, CommitInfo *> btree_author_name_;
  absl::btree_map<std::string, CommitInfo *> btree_date_;
};
