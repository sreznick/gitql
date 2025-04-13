#include <iostream>
#include <any>
#include <antlr4-runtime.h>
#include "TLexer.h"
#include "TParser.h"

int main() {
    antlr4::ANTLRInputStream input("6*(2+3)");
    gitql::TLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    gitql::TParser parser(&tokens);    
    std::cout << parser.expr()->toStringTree() << std::endl;
    return 0;
}
