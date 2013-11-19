#include "parser.h"

int main(){
	
	Parser* parser = new Parser(std::cin, std::cout);
	parser->parse();
	int errs = parser->totalErrors();
	delete parser;
	if(errs == 0){
		//std::cout << "No errors.\n";
	}
	else{
		std::cout << "Number of errors: " << errs << ".\n";
	}
	return 0;
}
