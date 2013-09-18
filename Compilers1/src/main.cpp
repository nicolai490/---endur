#include "scanner.h"
#include "types.h"
#include "lex.yy.cc"

int main(){
	FlexLexer* m_lexer = new yyFlexLexer(&std::cin, &std::cout);
	int ret = m_lexer->yylex();
	printf("%d", ret);
	return 0;
}
