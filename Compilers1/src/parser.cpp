/*
 * parser.cpp
 *
 *  Created on: Oct 12, 2013
 *      Author: ubuntu
 */

#include "parser.h"
#include "types.h"

Parser::Parser(std::istream& input, std::ostream& output){
	m_symbolTable = new SymbolTable();
	m_symbolTable->insert(CodeFalse);
	m_symbolTable->insert(CodeTrue);
	m_lexan = new Scanner(m_symbolTable, input, output);
	m_totalErrors = 0;
	m_parserError = false;
	getToken();
}

Parser::~Parser(){
	delete m_symbolTable;
	delete m_lexan;
}

void Parser::parse(){
	parseProgram();
	match(tc_EOF);
}

SymbolTable* Parser::getSymbolTable(){
	return m_symbolTable;
}

int Parser::totalErrors(){
	return m_totalErrors;
}

bool Parser::tokenCodeIn(TokenCode tc, const TokenCode list[]){
	int i = 0;
	int val = list[0];
	while(val != tc_NONE){
		if(tc == val){
			return true;
		}
		val = list[++i];
	}
	return false;
}

void Parser::recover(const TokenCode list[]){
	while(! tokenCodeIn(m_currentToken->getTokenCode(), list)){
		if(m_currentToken->getTokenCode() == tc_EOF) return;
		getToken();
	}
	m_parserError = false;
}

void Parser::getToken(){
	m_currentToken = m_lexan->nextToken();
	while(m_currentToken->getTokenCode() == tc_ERROR || m_currentToken->getTokenCode() == tc_ERROR2){
		if(m_currentToken->getTokenCode() == tc_ERROR){
			setError("Invalid Character.");
		}
		else{
			setError("Identifier too long.");
		}
		m_currentToken = m_lexan->nextToken();
		m_parserError = false;
	}
}

void Parser::match(TokenCode tc){
	if(getTokenCode() == tc){
		getToken();
	}
	else{
		expectedTokenCode(tc);
	}
}

void Parser::setError(const std::string& err){
	m_totalErrors++;
	m_lexan->addError(err);
	m_parserError = true;
}

void Parser::expectedTokenCode(TokenCode tc){
	setError("Expected " + tokenCodeStrings[tc] + ".");
}

TokenCode Parser::getTokenCode(){
	return m_currentToken->getTokenCode();
}

bool Parser::isNext(TokenCode tc){
	return (getTokenCode() == tc);
}

void Parser::parseProgram(){
	match(tc_PROGRAM);
	if(m_parserError){
		TokenCode synch[] = {tc_ID, tc_NONE};
		recover(synch);
//		if(isNext(tc_PROGRAM)){
//			match(tc_PROGRAM);
//		}
	}
	match(tc_ID);
	if(m_parserError){
		TokenCode synch[] = {tc_SEMICOL, tc_NONE};
		recover(synch);
//		if(isNext(tc_ID)){
//			match(tc_ID);
//		}
	}
	match(tc_SEMICOL);
	if(m_parserError){
		TokenCode synch[] = {tc_VAR, tc_FUNCTION, tc_PROCEDURE, tc_BEGIN, tc_NONE};
		recover(synch);
//		if(isNext(tc_SEMICOL)){
//			match(tc_SEMICOL);
//		}
	}
	parseDeclarations();
	parseSubprogramDeclarations();
	parseCompoundStatement();
	match(tc_DOT);
	if(m_parserError){
		TokenCode synch[] = {tc_EOF, tc_NONE};
		recover(synch);
//		if(isNext(tc_DOT)){
//			match(tc_DOT);
//		}
	}
}

void Parser::parseIdentifierList(EntryList& idList){
	match(tc_ID);
	if(m_parserError){
		TokenCode synch[] = {tc_COMMA, tc_COLON, tc_NONE};
		recover(synch);
//		if(isNext(tc_ID)){
//			match(tc_ID);
//		}
	}
	parseIdentifierListPrime(idList);
}

void Parser::parseIdentifierListPrime(EntryList& idList){
	if(isNext(tc_COMMA)){
		match(tc_COMMA);
		match(tc_ID);
		if(m_parserError){
			TokenCode synch[] = {tc_COMMA, tc_COLON, tc_NONE};
			recover(synch);
//			if(isNext(tc_ID)){
//				match(tc_ID);
//			}
		}
		parseIdentifierListPrime(idList);
	}
}

void Parser::parseDeclarations(){
	if(isNext(tc_VAR)){
		match(tc_VAR);
		parseIdentifierList(*(new EntryList()));
		match(tc_COLON);
		if(m_parserError){
			TokenCode synch[] = {tc_ARRAY, tc_INTEGER, tc_REAL, tc_NONE};
			recover(synch);
//			if(isNext(tc_COLON)){
//				match(tc_COLON);
//			}
		}
		parseType();
		match(tc_SEMICOL);
		if(m_parserError){
			TokenCode synch[] = {tc_VAR, tc_FUNCTION, tc_BEGIN, tc_PROCEDURE, tc_NONE};
			recover(synch);
//			if(isNext(tc_SEMICOL)){
//				match(tc_SEMICOL);
//			}
		}
		parseDeclarations();
	}
}

void Parser::parseType(){
	if(isNext(tc_ARRAY)){
		match(tc_ARRAY);
		if(m_parserError){
			TokenCode synch[] = {tc_LBRACKET, tc_NONE};
			recover(synch);
//			if(isNext(tc_ARRAY)){
//				match(tc_ARRAY);
//			}
		}
		match(tc_LBRACKET);
		if(m_parserError){
			TokenCode synch[] = {tc_NUMBER, tc_NONE};
			recover(synch);
//			if(isNext(tc_LBRACKET)){
//				match(tc_LBRACKET);
//			}
		}
		match(tc_NUMBER);
		if(m_parserError){
			TokenCode synch[] = {tc_DOTDOT, tc_NONE};
			recover(synch);
//			if(isNext(tc_NUMBER)){
//				match(tc_NUMBER);
//			}
		}
		match(tc_DOTDOT);
		if(m_parserError){
			TokenCode synch[] = {tc_NUMBER, tc_NONE};
			recover(synch);
//			if(isNext(tc_DOTDOT)){
//				match(tc_DOTDOT);
//			}
		}
		match(tc_NUMBER);
		if(m_parserError){
			TokenCode synch[] = {tc_RBRACKET, tc_NONE};
			recover(synch);
//			if(isNext(tc_NUMBER)){
//				match(tc_NUMBER);
//			}
		}
		match(tc_RBRACKET);
		if(m_parserError){
			TokenCode synch[] = {tc_OF, tc_NONE};
			recover(synch);
//			if(isNext(tc_RBRACKET)){
//				match(tc_RBRACKET);
//			}
		}
		match(tc_OF);
		if(m_parserError){
			TokenCode synch[] = {tc_INTEGER, tc_REAL, tc_NONE};
			recover(synch);
//			if(isNext(tc_OF)){
//				match(tc_OF);
//			}
		}
		parseStandardType();
	}
	else{
		parseStandardType();
	}
}

void Parser::parseStandardType(){
	if(isNext(tc_INTEGER)){
		match(tc_INTEGER);
	}
	else{
		match(tc_REAL);
		if(m_parserError){
			TokenCode synch[] = {tc_SEMICOL, tc_NONE};
			recover(synch);
//			if(isNext(tc_REAL)){
//				match(tc_REAL);
//			}
		}
	}
}

void Parser::parseSubprogramDeclarations(){
	if(isNext(tc_FUNCTION) || isNext(tc_PROCEDURE)){
		parseSubprogramDeclaration();
		match(tc_SEMICOL);
		if(m_parserError){
			TokenCode synch[] = {tc_FUNCTION, tc_PROCEDURE, tc_BEGIN, tc_NONE};
			recover(synch);
//			if(isNext(tc_SEMICOL)){
//				match(tc_SEMICOL);
//			}
		}
		parseSubprogramDeclarations();
	}
}

void Parser::parseSubprogramDeclaration(){
	parseSubprogramHead();
	parseDeclarations();
	parseCompoundStatement();
}

void Parser::parseSubprogramHead(){
	if(isNext(tc_FUNCTION)){
		match(tc_FUNCTION);
		match(tc_ID);
		if(m_parserError){
			TokenCode synch[] = {tc_LPAREN, tc_COLON, tc_NONE};
			recover(synch);
//			if(isNext(tc_ID)){
//				match(tc_ID);
//			}
		}
		parseArguments();
		match(tc_COLON);
		if(m_parserError){
			TokenCode synch[] = {tc_INTEGER, tc_REAL, tc_NONE};
			recover(synch);
//			if(isNext(tc_COLON)){
//				match(tc_COLON);
//			}
		}
		parseStandardType();
		match(tc_SEMICOL);
		if(m_parserError){
			TokenCode synch[] = {tc_VAR, tc_BEGIN, tc_NONE};
			recover(synch);
//			if(isNext(tc_SEMICOL)){
//				match(tc_SEMICOL);
//			}
		}
	}
	else{
		match(tc_PROCEDURE);
		if(m_parserError){
			TokenCode synch[] = {tc_ID, tc_NONE};
			recover(synch);
//			if(isNext(tc_PROCEDURE)){
//				match(tc_PROCEDURE);
//			}
		}
		match(tc_ID);
		if(m_parserError){
			TokenCode synch[] = {tc_LPAREN, tc_SEMICOL, tc_NONE};
			recover(synch);
//			if(isNext(tc_ID)){
//				match(tc_ID);
//			}
		}
		parseArguments();
		match(tc_SEMICOL);
		if(m_parserError){
			TokenCode synch[] = {tc_VAR, tc_BEGIN, tc_NONE};
			recover(synch);
//			if(isNext(tc_SEMICOL)){
//				match(tc_SEMICOL);
//			}
		}
	}
}

void Parser::parseArguments(){
	if(isNext(tc_LPAREN)){
		match(tc_LPAREN);
		parseParameterList();
		match(tc_RPAREN);
		if(m_parserError){
			TokenCode synch[] = {tc_COLON, tc_SEMICOL, tc_NONE};
			recover(synch);
//			if(isNext(tc_RPAREN)){
//				match(tc_RPAREN);
//			}
		}
	}
}

void Parser::parseParameterList(){
	parseIdentifierList(*(new EntryList()));
	match(tc_COLON);
	if(m_parserError){
		TokenCode synch[] = {tc_ARRAY, tc_INTEGER, tc_REAL, tc_NONE};
		recover(synch);
//		if(isNext(tc_COLON)){
//			match(tc_COLON);
//		}
	}
	parseType();
	parseParameterListPrime();
}

void Parser::parseParameterListPrime(){
	if(isNext(tc_SEMICOL)){
		match(tc_SEMICOL);
		parseIdentifierList(*(new EntryList()));
		match(tc_COLON);
		if(m_parserError){
			TokenCode synch[] = {tc_ARRAY, tc_INTEGER, tc_REAL, tc_NONE};
			recover(synch);
//			if(isNext(tc_COLON)){
//				match(tc_COLON);
//			}
		}
		parseType();
		parseParameterListPrime();
	}
}

void Parser::parseCompoundStatement(){
	match(tc_BEGIN);
	if(m_parserError){
		TokenCode synch[] = {tc_BEGIN, tc_IF, tc_WHILE, tc_ID, tc_END, tc_NONE};
		recover(synch);
//		if(isNext(tc_BEGIN)){
//			match(tc_BEGIN);
//		}
	}
	parseOptionalStatements();
	match(tc_END);
	if(m_parserError){
		TokenCode synch[] = {tc_END, tc_DOT, tc_SEMICOL, tc_ELSE, tc_NONE};
		recover(synch);
//		if(isNext(tc_END)){
//			match(tc_END);
//		}
	}
}

void Parser::parseOptionalStatements(){
	if(isNext(tc_ID) || isNext(tc_BEGIN) || isNext(tc_IF) || isNext(tc_WHILE)){
		parseStatementList();
	}
}

void Parser::parseStatementList(){
	parseStatement();
	parseStatementListPrime();
}

void Parser::parseStatementListPrime(){
	if(isNext(tc_SEMICOL)){
		match(tc_SEMICOL);
		parseStatement();
		parseStatementListPrime();
	}
}

void Parser::parseStatement(){
	if(isNext(tc_ID)){
		SymbolTableEntry* entry = m_currentToken->getSymTabEntry();
		match(tc_ID);
		parseStatementPrime(entry);
	}
	else if(isNext(tc_IF)){
		match(tc_IF);
		parseExpression();
		match(tc_THEN);
		if(m_parserError){
			TokenCode synch[] = {tc_ID, tc_BEGIN, tc_IF, tc_WHILE, tc_NONE};
			recover(synch);
//			if(isNext(tc_THEN)){
//				match(tc_THEN);
//			}
		}
		parseStatement();
		match(tc_ELSE);
		if(m_parserError){
			TokenCode synch[] = {tc_ID, tc_BEGIN, tc_IF, tc_WHILE, tc_NONE};
			recover(synch);
//			if(isNext(tc_ELSE)){
//				match(tc_ELSE);
//			}
		}
		parseStatement();
	}
	else if(isNext(tc_WHILE)){
		match(tc_WHILE);
		parseExpression();
		match(tc_DO);
		if(m_parserError){
			TokenCode synch[] = {tc_ID, tc_BEGIN, tc_IF, tc_WHILE, tc_NONE};
			recover(synch);
//			if(isNext(tc_DO)){
//				match(tc_DO);
//			}
		}
		parseStatement();
	}
	else{
		parseCompoundStatement();
	}
}

void Parser::parseStatementPrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_LBRACKET) || isNext(tc_ASSIGNOP)){
		parseVariablePrime(prevEntry);
		match(tc_ASSIGNOP);
		if(m_parserError){
			TokenCode synch[] = {tc_ID, tc_NUMBER, tc_LPAREN, tc_NOT, tc_ADDOP, tc_NONE};
			recover(synch);
//			if(isNext(tc_ASSIGNOP)){
//				match(tc_ASSIGNOP);
//			}
		}
		parseExpression();
	}
	else{
		parseProcedureStatementPrime(prevEntry);
	}
}

SymbolTableEntry* Parser::parseVariable(){
	SymbolTableEntry* entry = NULL;
	if(isNext(tc_ID)){
		entry = m_currentToken->getSymTabEntry();
	}
	match(tc_ID);
	if(m_parserError){
		TokenCode synch[] = {tc_LBRACKET, tc_END, tc_ASSIGNOP, tc_MULOP, tc_RELOP, tc_THEN, tc_DO, tc_COMMA, tc_RPAREN, tc_RBRACKET, tc_NONE};
		recover(synch);
//		if(isNext(tc_ID)){
//			match(tc_ID);
//		}
	}
	entry = parseVariablePrime(entry);
	return entry;
}

SymbolTableEntry* Parser::parseVariablePrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_LBRACKET)){
		match(tc_LBRACKET);
		prevEntry = parseExpression();
		match(tc_RBRACKET);
		if(m_parserError){
			TokenCode synch[] = {tc_RBRACKET, tc_END, tc_ASSIGNOP, tc_MULOP, tc_RELOP, tc_THEN, tc_DO, tc_COMMA, tc_RPAREN, tc_NONE};
			recover(synch);
//			if(isNext(tc_RBRACKET)){
//				match(tc_RBRACKET);
//			}
		}
	}
	return prevEntry;
}

void Parser::parseProcedureStatement(){
	SymbolTableEntry* entry = NULL;
	if(isNext(tc_ID)){
		entry = m_currentToken->getSymTabEntry();
	}
	match(tc_ID);
	if(m_parserError){
		TokenCode synch[] = {tc_LPAREN, tc_END, tc_SEMICOL, tc_ELSE, tc_NONE};
		recover(synch);
//		if(isNext(tc_ID)){
//			match(tc_ID);
//		}
	}
	parseProcedureStatementPrime(entry);
}

void Parser::parseProcedureStatementPrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_LPAREN)){
		match(tc_LPAREN);
		parseExpressionList(prevEntry);
		match(tc_RPAREN);
		if(m_parserError){
			TokenCode synch[] = {tc_END, tc_SEMICOL, tc_ELSE, tc_NONE};
			recover(synch);
//			if(isNext(tc_RPAREN)){
//				match(tc_RPAREN);
//			}
		}
	}
}

void Parser::parseExpressionList(SymbolTableEntry* prevEntry){
	EntryList expList = *(new EntryList());
	expList.push_back(prevEntry);
	expList.push_back(parseExpression());
	parseExpressionListPrime(expList);
}

void Parser::parseExpressionListPrime(EntryList& expList){
	if(isNext(tc_COMMA)){
		match(tc_COMMA);
		expList.push_back(parseExpression());
		parseExpressionListPrime(expList);
	}
}

SymbolTableEntry* Parser::parseExpression(){
	SymbolTableEntry* entry = parseSimpleExpression();
	entry = parseExpressionPrime(entry);
	return entry;
}

SymbolTableEntry* Parser::parseExpressionPrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_RELOP)){
		match(tc_RELOP);
		prevEntry = parseSimpleExpression();
	}
	return prevEntry;
}

SymbolTableEntry* Parser::parseSimpleExpression(){
	if(isNext(tc_ADDOP)){
		parseSign();
	}
	SymbolTableEntry* entry = parseTerm();
	entry = parseSimpleExpressionPrime(entry);
	return entry;
}

SymbolTableEntry* Parser::parseSimpleExpressionPrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_ADDOP)){
		match(tc_ADDOP);
		parseTerm();
		prevEntry = parseSimpleExpressionPrime(prevEntry);
	}
	return prevEntry;
}

SymbolTableEntry* Parser::parseTerm(){
	SymbolTableEntry* entry = parseFactor();
	entry = parseTermPrime(entry);
	return entry;
}

SymbolTableEntry* Parser::parseTermPrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_MULOP)){
		match(tc_MULOP);
		parseFactor();
		prevEntry = parseTermPrime(prevEntry);
	}
	return prevEntry;
}

SymbolTableEntry* Parser::parseFactor(){
	SymbolTableEntry* entry = NULL;
	if(isNext(tc_ID)){
		entry = m_currentToken->getSymTabEntry();
		match(tc_ID);
		entry = parseFactorPrime(entry);
	}
	else if(isNext(tc_LPAREN)){
		match(tc_LPAREN);
		entry = parseExpression();
		match(tc_RPAREN);
		if(m_parserError){
			TokenCode synch[] = {tc_RPAREN, tc_END, tc_ADDOP, tc_MULOP, tc_RELOP, tc_THEN, tc_DO, tc_COMMA, tc_RBRACKET, tc_SEMICOL, tc_ELSE, tc_NONE};
			recover(synch);
//			if(isNext(tc_RPAREN)){
//				match(tc_RPAREN);
//			}
		}
	}
	else if(isNext(tc_NOT)){
		match(tc_NOT);
		parseFactor();
	}
	else{
		match(tc_NUMBER);
		if(m_parserError){
			TokenCode synch[] = {tc_RPAREN, tc_END, tc_ADDOP, tc_MULOP, tc_RELOP, tc_THEN, tc_DO, tc_COMMA, tc_RBRACKET, tc_SEMICOL, tc_ELSE, tc_NONE};
			recover(synch);
//			if(isNext(tc_NUMBER)){
//				match(tc_NUMBER);
//			}
		}
	}
	return entry;
}

SymbolTableEntry* Parser::parseFactorPrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_LBRACKET)){
		prevEntry = parseVariablePrime(prevEntry);
	}
	else if(isNext(tc_LPAREN)){
		match(tc_LPAREN);
		parseExpressionList(prevEntry);
		match(tc_RPAREN);
		if(m_parserError){
			TokenCode synch[] = {tc_RPAREN, tc_END, tc_ADDOP, tc_MULOP, tc_RELOP, tc_THEN, tc_DO, tc_COMMA, tc_RBRACKET, tc_SEMICOL, tc_ELSE, tc_NONE};
			recover(synch);
//			if(isNext(tc_RPAREN)){
//				match(tc_RPAREN);
//			}
		}
	}
	return prevEntry;
}

void Parser::parseSign(){
	OpType t = m_currentToken->getOpType();
	match(tc_ADDOP);
	if(m_parserError || (t != op_PLUS && t != op_MINUS)){
		if(!m_parserError){
			setError("Expected a sign.");
		}
		TokenCode synch[] = {tc_ID, tc_NUMBER, tc_NOT, tc_NONE};
		recover(synch);
	}
}
