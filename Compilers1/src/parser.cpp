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
	m_lexan = new Scanner(m_symbolTable, input, output);
	getToken();
	m_totalErrors = 0;
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
}

void Parser::getToken(){
	m_currentToken = m_lexan->nextToken();
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
}

void Parser::expectedTokenCode(TokenCode tc){
	setError("Inserted " + tokenCodeStrings[tc] + ".");
}

TokenCode Parser::getTokenCode(){
	return m_currentToken->getTokenCode();
}

bool Parser::isNext(TokenCode tc){
	return (getTokenCode() == tc);
}

void Parser::parseProgram(){
	const TokenCode synch[] = {tc_PROGRAM, tc_EOF, tc_NONE};
	if(! isNext(tc_PROGRAM)){
		setError("Expected a program.");
		recover(synch);
		if(isNext(tc_EOF)){
			return;
		}
	}
	match(tc_PROGRAM);
	match(tc_ID);
	match(tc_SEMICOL);
	parseDeclarations();
	parseSubprogramDeclarations();
	parseCompoundStatement();
	match(tc_DOT);
}

void Parser::parseIdentifierList(EntryList& idList){
	const TokenCode synch[] = {tc_ID, tc_COLON, tc_NONE};
	if(! isNext(tc_ID)){
		setError("Expected an identifier.");
		recover(synch);
		if(isNext(tc_COLON)){
			return;
		}
	}
	match(tc_ID);
	parseIdentifierListPrime(idList);
}

void Parser::parseIdentifierListPrime(EntryList& idList){
	if(isNext(tc_COMMA)){
		match(tc_COMMA);
		match(tc_ID);
		parseIdentifierList(idList);
	}
}

void Parser::parseDeclarations(){
	if(isNext(tc_VAR)){
		match(tc_VAR);
		parseIdentifierList(*(new EntryList()));
		match(tc_COLON);
		parseType();
		match(tc_SEMICOL);
		parseDeclarations();
	}
}

void Parser::parseType(){
	const TokenCode synch[] = {tc_ARRAY, tc_INTEGER, tc_REAL, tc_SEMICOL, tc_NONE};
	if(! (isNext(tc_ARRAY) || isNext(tc_INTEGER) || isNext(tc_REAL))){
		setError("Expected a type.");
		recover(synch);
		if(isNext(tc_SEMICOL)){
			return;
		}
	}
	if(isNext(tc_ARRAY)){
		match(tc_ARRAY);
		match(tc_LBRACKET);
		match(tc_NUMBER);
		match(tc_DOTDOT);
		match(tc_NUMBER);
		match(tc_RBRACKET);
		match(tc_OF);
		parseStandardType();
	}
	else{
		parseStandardType();
	}
}

void Parser::parseStandardType(){
	const TokenCode synch[] = {tc_INTEGER, tc_REAL, tc_SEMICOL, tc_NONE};
	if(! (isNext(tc_INTEGER) || isNext(tc_REAL))){
		setError("Expected a standard type.");
		recover(synch);
		if(isNext(tc_SEMICOL)){
			return;
		}
	}
	if(isNext(tc_INTEGER)){
		match(tc_INTEGER);
	}
	else if(isNext(tc_REAL)){
		match(tc_REAL);
	}
}

void Parser::parseSubprogramDeclarations(){
	if(isNext(tc_FUNCTION) || isNext(tc_PROCEDURE)){
		parseSubprogramDeclaration();
		match(tc_SEMICOL);
		parseSubprogramDeclarations();
	}
}

void Parser::parseSubprogramDeclaration(){
	const TokenCode synch[] = {tc_FUNCTION, tc_PROCEDURE, tc_SEMICOL, tc_NONE};
	if(! (isNext(tc_FUNCTION) || isNext(tc_PROCEDURE))){
		setError("Expected a subprogram declaration.");
		recover(synch);
		if(isNext(tc_SEMICOL)){
			return;
		}
	}
	parseSubprogramHead();
	parseDeclarations();
	parseCompoundStatement();
}

void Parser::parseSubprogramHead(){
	const TokenCode synch[] = {tc_FUNCTION, tc_PROCEDURE, tc_VAR, tc_BEGIN, tc_NONE};
	if(! (isNext(tc_FUNCTION) || isNext(tc_PROCEDURE))){
		setError("Expected a subprogram head.");
		recover(synch);
		if(isNext(tc_VAR) || isNext(tc_BEGIN)){
			return;
		}
	}
	if(isNext(tc_FUNCTION)){
		match(tc_FUNCTION);
		match(tc_ID);
		parseArguments();
		match(tc_COLON);
		parseStandardType();
		match(tc_SEMICOL);
	}
	else if(isNext(tc_PROCEDURE)){
		match(tc_PROCEDURE);
		match(tc_ID);
		parseArguments();
		match(tc_SEMICOL);
	}
}

void Parser::parseArguments(){
	if(isNext(tc_LPAREN)){
		match(tc_LPAREN);
		parseParameterList();
		match(tc_RPAREN);
	}
}

void Parser::parseParameterList(){
	const TokenCode synch[] = {tc_ID, tc_RPAREN, tc_NONE};
	if(! isNext(tc_ID)){
		setError("Expected a parameter list.");
		recover(synch);
		if(isNext(tc_RPAREN)){
			return;
		}
	}
	parseIdentifierList(*(new EntryList()));
	match(tc_COLON);
	parseType();
	parseParameterListPrime();
}

void Parser::parseParameterListPrime(){
	if(isNext(tc_SEMICOL)){
		match(tc_SEMICOL);
		parseIdentifierList(*(new EntryList()));
		match(tc_COLON);
		parseType();
		parseParameterListPrime();
	}
}

void Parser::parseCompoundStatement(){
	const TokenCode first[] = {tc_BEGIN, tc_DOT, tc_SEMICOL, tc_ELSE, tc_NONE};
	if(! isNext(tc_BEGIN)){
		setError("Expected a compound statement.");
		recover(synch);
		if(isNext(tc_DOT) || isNext(tc_SEMICOL) || isNext(tc_ELSE)){
			return;
		}
	}
	match(tc_BEGIN);
	parseOptionalStatements();
	match(tc_END);
}

void Parser::parseOptionalStatements(){
	if(isNext(tc_ID) || isNext(tc_BEGIN) || isNext(tc_IF) || isNext(tc_WHILE)){
		parseStatementList();
	}
}

void Parser::parseStatementList(){
	const TokenCode synch[] = {tc_BEGIN, tc_IF, tc_WHILE, tc_ID, tc_END, tc_NONE};
	if(! (isNext(tc_BEGIN) || isNext(tc_IF) || isNext(tc_WHILE) || isNext(tc_ID))){
		setError("Expected a statement list.");
		recover(synch);
		if(isNext(tc_END)){
			return;
		}
	}
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
	const TokenCode synch[] = {tc_BEGIN, tc_IF,tc_WHILE, tc_ID, tc_SEMICOL, tc_ELSE, tc_NONE};
	if(! (isNext(tc_BEGIN) || isNext(tc_IF) || isNext(tc_ID))){
		setError("Expected a statement.");
		recover(synch);
		if(isNext(tc_SEMICOL) || isNext(tc_ELSE)){
			return;
		}
	}
	if(isNext(tc_ID)){
		SymbolTableEntry* entry = m_currentToken->getSymTabEntry();
		match(tc_ID);
		parseStatementPrime(entry);
	}
	else if(isNext(tc_BEGIN)){
		parseCompoundStatement();
	}
	else if(isNext(tc_IF)){
		match(tc_IF);
		parseExpression();
		match(tc_THEN);
		parseStatement();
		match(tc_ELSE);
		parseStatement();
	}
	else if(isNext(tc_WHILE)){
		match(tc_WHILE);
		parseExpression();
		match(tc_DO);
		parseStatement();
	}
}

void Parser::parseStatementPrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_LBRACKET) || isNext(tc_ASSIGNOP)){
		parseVariablePrime(prevEntry);
		match(tc_ASSIGNOP);
		parseExpression();
	}
	else{
		parseProcedureStatementPrime(prevEntry);
	}
}

SymbolTableEntry* Parser::parseVariable(){
	const TokenCode synch[] = {tc_ID, tc_ASSIGNOP, tc_MULOP, tc_ADDOP, tc_RELOP, tc_THEN, tc_DO, tc_RBRACKET, tc_COMMA, tc_RPAREN, tc_SEMICOL, tc_ELSE, tc_NONE};
	if(! isNext(tc_ID)){
		setError("Expected a variable.");
		recover(synch);
		if(isNext(tc_ASSIGNOP) || isNext(tc_MULOP) || isNext(tc_ADDOP) || isNext(tc_RELOP) || isNext(tc_THEN) || isNext(tc_DO) || isNext(tc_RBRACKET) || isNext(tc_COMMA) || isNext(tc_RPAREN) || isNext(tc_SEMICOL) || isNext(tc_ELSE)){
			return NULL;
		}
	}
	SymbolTableEntry* entry = NULL;
	if(isNext(tc_ID)){
		entry = m_currentToken->getSymTabEntry();
	}
	match(tc_ID);
	entry = parseVariablePrime(entry);
	return entry;
}

SymbolTableEntry* Parser::parseVariablePrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_LBRACKET)){
		match(tc_LBRACKET);
		prevEntry = parseExpression();
		match(tc_RBRACKET);
	}
	return prevEntry;
}

void Parser::parseProcedureStatement(){
	const TokenCode synch[] = {tc_ID, tc_SEMICOL, tc_ELSE, tc_NONE};
	if(! isNext(tc_ID)){
		setError("Expected a procedure statement.");
		recover(synch);
		if(isNext(tc_SEMICOL) || isNext(tc_ELSE)){
			return;
		}
	}
	SymbolTableEntry* entry = NULL;
	if(isNext(tc_ID)){
		entry = m_currentToken->getSymTabEntry();
	}
	match(tc_ID);
	parseProcedureStatementPrime(entry);
}

void Parser::parseProcedureStatementPrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_LPAREN)){
		match(tc_LPAREN);
		parseExpressionList(prevEntry);
		match(tc_RPAREN);
	}
}

void Parser::parseExpressionList(SymbolTableEntry* prevEntry){
	const TokenCode synch[] = {tc_ID, tc_NUMBER, tc_LPAREN, tc_NOT, tc_ADDOP, tc_RPAREN, tc_NONE};
	if(! (isNext(tc_ID) || isNext(tc_NUMBER) || isNext(tc_LPAREN) || isNext(tc_NOT) || isNext(tc_ADDOP))){
		setError("Expected an expression list.");
		recover(synch);
		if(isNext(tc_RPAREN)){
			return;
		}
	}
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
	const TokenCode synch[] = {tc_ID, tc_NUMBER, tc_LPAREN, tc_NOT, tc_ADDOP, tc_SEMICOL, tc_ELSE, tc_THEN, tc_DO, tc_RBRACKET, tc_COMMA, tc_RPAREN, tc_NONE};
	if(! (isNext(tc_ID) || isNext(tc_NUMBER) || isNext(tc_LPAREN) || isNext(tc_NOT) || isNext(tc_ADDOP))){
		setError("Expected an expression.");
		recover(synch);
		if(isNext(tc_SEMICOL) || isNext(tc_ELSE) || isNext(tc_THEN) || isNext(tc_DO) || isNext(tc_RBRACKET) || isNext(tc_COMMA) || isNext(tc_RPAREN)){
			return NULL;
		}
	}
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
	const TokenCode synch[] = {tc_ID, tc_NUMBER, tc_LPAREN, tc_NOT, tc_ADDOP, tc_SEMICOL, tc_ELSE, tc_THEN, tc_DO, tc_RBRACKET, tc_COMMA, tc_RPAREN, tc_RELOP, tc_NONE};
		if(! (isNext(tc_ID) || isNext(tc_NUMBER) || isNext(tc_LPAREN) || isNext(tc_NOT) || isNext(tc_ADDOP))){
			setError("Expected a simple expression.");
			recover(synch);
			if(isNext(tc_SEMICOL) || isNext(tc_ELSE) || isNext(tc_THEN) || isNext(tc_DO) || isNext(tc_RBRACKET) || isNext(tc_COMMA) || isNext(tc_RPAREN) || isNext(tc_RELOP)){
				return NULL;
			}
		}
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
	const TokenCode synch[] = {tc_ID, tc_NUMBER, tc_LPAREN, tc_NOT, tc_ADDOP, tc_RELOP, tc_THEN, tc_DO, tc_RBRACKET, tc_COMMA, tc_RPAREN, tc_SEMICOL, tc_ELSE, tc_NONE};
	if(! (isNext(tc_ID) || isNext(tc_NUMBER) || isNext(tc_LPAREN) || isNext(tc_NOT))){
		setError("Expected a term.");
		recover(synch);
		if(isNext(tc_ADDOP) || isNext(tc_RELOP) || isNext(tc_THEN) || isNext(tc_DO) || isNext(tc_RBRACKET) || isNext(tc_COMMA) || isNext(tc_RPAREN) || isNext(tc_SEMICOL) || isNext(tc_ELSE)){
			return NULL;
		}
	}
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
	const TokenCode synch[] = {tc_ID, tc_NUMBER, tc_LPAREN, tc_NOT, tc_MULOP, tc_ADDOP, tc_RELOP, tc_THEN, tc_DO, tc_RBRACKET, tc_COMMA, tc_RPAREN, tc_SEMICOL, tc_ELSE, tc_NONE};
	if(! (isNext(tc_ID) || isNext(tc_NUMBER) || isNext(tc_LPAREN) || isNext(tc_NOT))){
		setError("Expected a factor.");
		recover(synch);
		if(isNext(tc_ADDOP) || isNext(tc_RELOP) || isNext(tc_THEN) || isNext(tc_DO) || isNext(tc_RBRACKET) || isNext(tc_COMMA) || isNext(tc_RPAREN) || isNext(tc_SEMICOL) || isNext(tc_ELSE) || isNext(tc_MULOP)){
			return NULL;
		}
	}
	SymbolTableEntry* entry = NULL;
	if(isNext(tc_ID)){
		entry = m_currentToken->getSymTabEntry();
		match(tc_ID);
		entry = parseFactorPrime(entry);
	}
	else if(isNext(tc_NUMBER)){
		match(tc_NUMBER);
	}
	else if(isNext(tc_LPAREN)){
		match(tc_LPAREN);
		entry = parseExpression();
		match(tc_RPAREN);
	}
	else if(isNext(tc_NOT)){
		match(tc_NOT);
		parseFactor();
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
	}
	return prevEntry;
}

void Parser::parseSign(){
	const TokenCode synch[] = {tc_ADDOP, tc_ID, tc_NUMBER, tc_NOT, tc_NONE};
	while(!(m_currentToken->getOpType() == op_PLUS || m_currentToken->getOpType() == op_MINUS)){
		setError("Expected a sign.");
		recover(synch);
		if(isNext(tc_ID) || isNext(tc_NUMBER) || isNext(tc_NOT)){
			return;
		}
	}
	match(tc_ADDOP);
}
