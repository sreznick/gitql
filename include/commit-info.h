#pragma once
#include "constants.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <ctime>
#include <git2/types.h>
#include <string>
#include <vector>
// struct CommitSha {
//   std::string sha;
//   bool operator<(const CommitSha &rhs) { return sha < rhs.sha; }
// };

struct CommitInfo {
  CommitInfo(const char *hash, const git_signature *author, const char *message,
             size_t date, const std::vector<std::string> &files);
  //std::array<char, constants::HashSize> hash;
  std::string hash;
  std::string author_name;
  std::string author_email;
  std::string message;
  //std::vector<std::array<char, constants::HashSize + 1>> files;
  std::vector<std::string> files;
  //std::string date;
  std::chrono::seconds date;
};

// struct Repo {
//   std::string path;
//   git_repository *repo;
//   git_oid head_oid;
// };

// struct Node {
  
// };
