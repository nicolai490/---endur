#include "parser.h"

int main(){
	Parser* parser = new Parser(std::cin, std::cout);
	parser->parse();
	if(parser->totalErrors() == 0){
		std::cout << "No errors.\n";
	}
	else{
		std::cout << "Number of errors: " << parser->totalErrors() << ".\n";
	}
	delete parser;
	return 0;
}
