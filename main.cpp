#include "TLexer.h"
#include "TParser.h"
#include "commits-proc.h"
#include "config.hpp"
#include "git2.h"
#include "query-handler.h"
#include "query.hpp"
#include <algorithm>
#include <antlr4-runtime.h>
#include <any>
#include <cstdlib>
#include <iomanip>
#include <iostream>
void PrintHelp() {
  std::cout << "help:" << std::endl;
  std::cout << "\t -query: user request to gitql" << std::endl;
  std::cout << "\t -help: all possible arguments" << std::endl;
}

void PrintQuery(const Query &query) {
  std::cout << "From: " << query.From() << std::endl;
  std::cout << "Limit: " << query.Limit() << std::endl;
  std::cout << "Offset: " << query.Offset() << std::endl;
  std::cout << "Select: " << std::bitset<64>(query.Select()) << std::endl;
  for (std::size_t i = 0; i < query.Where().size(); i++) {
    std::cout << "Where (mask): " << query.Where()[i].Type << std::endl;
    std::cout << "Where (from time): "
              << std::put_time(&query.Where()[i].FromTime, "%Y-%m-%d %H:%M:%S")
              << std::endl;
    std::cout << "Where (to time): "
              << std::put_time(&query.Where()[i].ToTime, "%Y-%m-%d %H:%M:%S")
              << std::endl;
    std::cout << "Where (from time seconds): "
              << query.Where()[i].FromTimeSeconds() << std::endl;
    std::cout << "Where (to time seconds): " << query.Where()[i].ToTimeSeconds()
              << std::endl;
    std::cout << "Where (to time): "
              << std::put_time(&query.Where()[i].ToTime, "%Y-%m-%d %H:%M:%S")
              << std::endl;
    std::cout << "Where (key): " << query.Where()[i].Key << std::endl;
    std::cout << "Where (value): " << query.Where()[i].Value << std::endl;
  }
}

void PrintTable(const std::vector<std::vector<std::string>> &table) {
  if (table.empty())
    return;
  size_t cols = table[0].size();
  std::vector<size_t> widths(cols, 0);
  for (const auto &row : table) {
    for (size_t i = 0; i < row.size(); ++i) {
      widths[i] = std::max(widths[i], row[i].size());
    }
  }
  auto printSeparator = [&]() {
    std::cout << '+';
    for (size_t w : widths) {
      std::cout << std::string(w + 2, '-') << '+';
    }
    std::cout << "\n";
  };

  printSeparator();
  for (const auto &row : table) {
    std::cout << '|';
    for (size_t i = 0; i < row.size(); ++i) {
      std::cout << ' ' << std::setw(widths[i]) << std::left << row[i] << ' '
                << '|';
    }
    std::cout << "\n";
    printSeparator();
  }
}

int main(int argc, char *argv[]) {

  try {
    Config config(argc, argv);
    if (config.Help()) {
      // PrintHelp();
      return EXIT_SUCCESS;
    }
    antlr4::ANTLRInputStream input(config.Query());
    gitql::TLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    gitql::TParser parser(&tokens);
    Query query(parser.main());
    // PrintQuery(query);
    git_libgit2_init();
    git_repository *repo = nullptr;
    if (git_repository_open(&repo, "/home/sh4rrkyyyy/os/xxxx") != 0) {
      std::cerr << "Incorrect path" << std::endl;
      return EXIT_FAILURE;
    }
    std::vector<std::unique_ptr<CommitInfo>> vec;
    try {
      vec = GetAllCommitsInfo(repo, "lab7");
    } catch (std::exception &e) {
      std::cout << e.what() << std::endl;
    }

    QueryHandler qh(repo, vec);
    auto res = qh.Execute(query);
    if (res.size() > 0 && res.size() > query.Limit()) {
      res.resize(query.Limit());
    }
    PrintTable(res);

    git_repository_free(repo);
    git_libgit2_shutdown();

  } catch (std::invalid_argument err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
