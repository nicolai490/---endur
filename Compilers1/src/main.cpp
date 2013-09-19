#include "scanner.h"

int main(){
	SymbolTable* symbolTable = new SymbolTable();
	Scanner* scanner = new Scanner(symbolTable, std::cin, std::cout);
	Token* token = scanner->nextToken();
	while(token->getTokenCode() != tc_EOF){
		token = scanner->nextToken();
		std::cout << Token::toString(token) << " ";
		token->~Token();
	}
	std::cout << "\n\n";
	SymbolTable::print(symbolTable);
}
