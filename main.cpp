#include <iostream>
#include <any>
#include <antlr4-runtime.h>
#include "TLexer.h"
#include "TParser.h"
#include "config.hpp"
#include "query.hpp"

void PrintHelp() {
    std::cout << "help:" << std::endl;
    std::cout << "\t -query: user request to gitql" << std::endl;
    std::cout << "\t -help: all possible arguments" << std::endl;
}

void PrintQuery(const Query& query) {
    std::cout << "From: " << query.From() << std::endl;
    std::cout << "Limit: " << query.Limit() << std::endl;
    std::cout << "Offset: " << query.Offset() << std::endl;
    std::cout << "Select: " << std::bitset<64>(query.Select()) << std::endl;
    for (std::size_t i = 0; i < query.Where().size(); i++) {
        std::cout << "Where (mask): " << query.Where()[i].Type << std::endl;
        std::cout << "Where (from time): " << std::put_time(&query.Where()[i].FromTime, "%Y-%m-%d %H:%M:%S") << std::endl;
        std::cout << "Where (to time): " << std::put_time(&query.Where()[i].ToTime, "%Y-%m-%d %H:%M:%S") << std::endl;
        std::cout << "Where (from time seconds): " << query.Where()[i].FromTimeSeconds() << std::endl;
        std::cout << "Where (to time seconds): " << query.Where()[i].ToTimeSeconds() << std::endl;
        std::cout << "Where (to time): " << std::put_time(&query.Where()[i].ToTime, "%Y-%m-%d %H:%M:%S") << std::endl;
        std::cout << "Where (key): " << query.Where()[i].Key << std::endl;
        std::cout << "Where (value): " << query.Where()[i].Value << std::endl;
    }
}

int main(int argc, char* argv[]) {
    try {
        Config config(argc, argv);
        if (config.Help()) {
            PrintHelp();
            return EXIT_SUCCESS;
        }
        antlr4::ANTLRInputStream input(config.Query());
        gitql::TLexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);
        gitql::TParser parser(&tokens);
        Query query(parser.main());
        PrintQuery(query);
        // execute query
    } catch(std::invalid_argument err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
