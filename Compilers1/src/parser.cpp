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
}

SymbolTable* Parser::getSymbolTable(){
	return m_symbolTable;
}

int Parser::totalErrors(){
	return m_totalErrors;
}

bool Parser::tokenCodeIn(TokenCode tc, const TokenCode list[]){
	return false;
}

void Parser::recover(const TokenCode list[]){
	
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
		getToken();
	}
}

void Parser::setError(const std::string& err){
	m_totalErrors++;
	m_lexan->addError(err);
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
	match(tc_ID);
	match(tc_SEMICOL);
	parseDeclarations();
	parseSubprogramDeclarations();
	parseCompoundStatement();
	match(tc_DOT);
}

void Parser::parseIdentifierList(EntryList& idList){
	match(tc_ID);
	parseIdentifierListPrime(idList);
}

void Parser::parseIdentifierListPrime(EntryList& idList){
	match(tc_COMMA);
	match(tc_ID);
	if(isNext(tc_COMMA)){
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
	if(isNext(tc_INTEGER)){
		match(tc_INTEGER);
	}
	else if(isNext(tc_REAL)){
		match(tc_REAL);
	}
	else{
		setError("Expected a type.");
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
	parseSubprogramHead();
	parseDeclarations();
	parseCompoundStatement();
}

void Parser::parseSubprogramHead(){
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
	else{
		setError("Expected a subprogram declaration.");
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
	else{
		setError("Expected a statement.");
	}
}

void Parser::parseStatementPrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_LBRACKET)){
		parseVariablePrime(prevEntry);
	}
	else if(isNext(tc_LPAREN)){
		parseProcedureStatementPrime(prevEntry);
	}
}

SymbolTableEntry* Parser::parseVariable(){
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
	EntryList expList = *(new EntryList());
	expList.push_back(prevEntry);
	expList.push_back(parseExpression());
	parseExpressionListPrime(expList);
}

void Parser::parseExpressionListPrime(EntryList& expList){
	match(tc_COMMA);
	expList.push_back(parseExpression());
	parseExpressionListPrime(expList);
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
	match(tc_ADDOP);
	parseTerm();
	prevEntry = parseSimpleExpressionPrime(prevEntry);
	return prevEntry;
}

SymbolTableEntry* Parser::parseTerm(){
	SymbolTableEntry* entry = parseFactor();
	entry = parseTermPrime(entry);
	return entry;
}

SymbolTableEntry* Parser::parseTermPrime(SymbolTableEntry* prevEntry){
	match(tc_MULOP);
	parseFactor();
	prevEntry = parseTermPrime(prevEntry);
	return prevEntry;
}

SymbolTableEntry* Parser::parseFactor(){
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
	else{
		setError("Expected a factor.");
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
	if(!(m_currentToken->getOpType() == op_PLUS || m_currentToken->getOpType() == op_MINUS)){
		setError("Expected a sign.");
	}
	match(tc_ADDOP);
}
