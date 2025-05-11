#include "index-store.h"
#include "absl/container/btree_map.h"
#include "constants.h"
#include "query.hpp"
#include <unordered_set>
#include <vector>

std::vector<std::vector<std::string>> IndexStore::Execute(const Query &query) {
  std::vector<std::vector<std::string>> res;
  if (query.From() == COMMITS) {
    filterCommits(query);
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

void IndexStore::filterCommits(const Query &query) {
  std::unordered_set<CommitInfo *> s;
  for (size_t i = 0; i < query.Where().size(); ++i) {
    std::unordered_set<CommitInfo *> sn;
    auto key = query.Where()[i].Key, value = query.Where()[i].Value;
    absl::btree_map<std::string, CommitInfo *>::iterator low, high;
    if (key == "author_name") {
      low = btree_author_name_.lower_bound(value);
      high = btree_author_name_.upper_bound(value);
    } else if (key == "date") {
      low = btree_date_.lower_bound(value);
      high = btree_date_.upper_bound(value);
    }
    for (auto it = low; it != high; ++it) {
      if (i == 0 || s.contains(it->second)) {
        sn.insert(it->second);
      }
    }
    s = std::move(sn);
  }
  commits_ = std::move(s);
}
