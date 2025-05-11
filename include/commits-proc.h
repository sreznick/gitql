#pragma once
#include "constants.h"
#include "commit-info.h"
#include <cstddef>
#include <git2/types.h>
#include <string>
#include <vector>
#include <optional>
#include <memory>

std::optional<std::vector<std::unique_ptr<CommitInfo>>>
GetAllCommitsInfo(git_repository *, const std::string &);

std::optional<std::vector<std::string>>
GetCommitFiles(git_commit *, git_repository *);
