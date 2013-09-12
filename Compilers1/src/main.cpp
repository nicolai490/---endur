#include "types.h"
#include "symtab.h"
#include "token.h"
#include "scanner.h"
#include "lex.yy.cc"

int main(){
    FlexLexer* m_lexer = new yyFlexLexer();
    m_lexer->yylex();
    return 0;
}
