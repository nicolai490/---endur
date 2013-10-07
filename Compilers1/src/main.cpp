#include "scanner.h"

int main(){
	SymbolTable* symbolTable = new SymbolTable();
	Scanner* scanner = new Scanner(symbolTable, std::cin, std::cout);
	Token* token = scanner->nextToken();
	while(token->getTokenCode() != tc_EOF){
		std::cout << Token::toString(token) << " ";
		token = scanner->nextToken();
	}
	std::cout << Token::toString(token) << " ";
	std::cout << "\n\n";
	SymbolTable::print(symbolTable);
	delete symbolTable;
	delete scanner;
}
