#include "scanner.h"
#include "types.h"
#include "lex.yy.cc"

int main(){
	FlexLexer* m_lexer = new yyFlexLexer(&std::cin, &std::cout);
	int ret = 0;
	while(ret != 34){
	    ret = m_lexer->yylex();
	    printf("%d\n", ret);
	}
	return 0;
}
