#include "commit-info.h"
#include <chrono>

CommitInfo::CommitInfo(const char *hash, const git_signature *author,
                       const char *message, const std::string &date,
                       const std::vector<std::string> &files)
    : hash(std::string(hash)), author_name(std::string(author->name)),
      author_email(std::string(author->email)), message(std::string(message)),
      date(date), files(files) {}
