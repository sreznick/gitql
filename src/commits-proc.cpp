#include "commits-proc.h"
#include "commit-info.h"
#include "constants.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <git2/commit.h>
#include <git2/diff.h>
#include <git2/oid.h>
#include <git2/revwalk.h>
#include <git2/types.h>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

std::optional<std::vector<std::string>> GetCommitFiles(git_commit *commit,
                                                       git_repository *repo) {
  git_commit *parent = nullptr;
  git_commit_parent(&parent, commit, 0);
  git_tree *commit_tree, *parent_tree = nullptr;
  if (git_commit_tree(&commit_tree, commit) != 0) {
    git_commit_free(parent);
    return std::nullopt;
  }
  git_commit_tree(&parent_tree, parent);
  git_diff *diff;
  if (git_diff_tree_to_tree(&diff, repo, commit_tree, parent_tree, nullptr) !=
      0) {
    git_tree_free(commit_tree);
    git_commit_free(parent);
    git_tree_free(parent_tree);
    return std::nullopt;
  }
  std::vector<std::string> files;
  git_diff_foreach(
      diff,
      [](const git_diff_delta *delta, float, void *payload) {
        if (delta->new_file.mode != GIT_FILEMODE_BLOB ||
            delta->new_file.mode != GIT_FILEMODE_BLOB_EXECUTABLE) {
          return 0;
        }
        auto *files = (std::vector<std::string> *)payload;
        char buf[constants::HashSize + 1];
        git_oid_tostr(buf, sizeof(buf), &delta->new_file.id);
        files->emplace_back(buf);
        return 0;
      },
      nullptr, nullptr, nullptr, &files);
  git_diff_free(diff);
  git_commit_free(parent);
  git_tree_free(parent_tree);
  git_tree_free(commit_tree);
  return files;
}

std::optional<std::vector<std::unique_ptr<CommitInfo>>>
GetAllCommitsInfo(git_repository *repo, const std::string &branch) {
  git_revwalk *walker;
  if (git_revwalk_new(&walker, repo) != 0) {
    return std::nullopt;
  }
  auto refname = "refs/heads/" + branch;
  if (git_revwalk_push_ref(walker, refname.c_str()) != 0) {
    git_revwalk_free(walker);
    return std::nullopt;
  }
  git_oid oid;

  std::vector<std::unique_ptr<CommitInfo>> commits;
  while (git_revwalk_next(&oid, walker) == 0) {
    git_commit *commit;
    if (git_commit_lookup(&commit, repo, &oid) != 0) {
      git_revwalk_free(walker);
      return std::nullopt;
    }
    char hash[constants::HashSize + 1];
    git_oid_tostr(hash, sizeof(hash), &oid);
    auto author = git_commit_author(commit);
    auto message = git_commit_message(commit);
    auto time = git_commit_time(commit);
    auto files = GetCommitFiles(commit, repo);
    if (!files) {
      git_commit_free(commit);
      git_revwalk_free(walker);
      return std::nullopt;
    }
    commits.emplace_back(
        std::make_unique<CommitInfo>(hash, author, message, time, *files));
    git_commit_free(commit);
  }
  git_revwalk_free(walker);
  return commits;
}
