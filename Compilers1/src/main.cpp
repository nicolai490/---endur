#include "scanner.h"
#include "lex.yy.cc"

int main(){
    FlexLexer* m_lexer = new yyFlexLexer(&std::cin, &std::cout);
    m_lexer->yylex();
    return 0;
}
