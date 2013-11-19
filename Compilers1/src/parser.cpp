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
	m_code = new Code();
	getToken();
}

Parser::~Parser(){
	delete m_symbolTable;
	delete m_lexan;
	delete m_code;
}

void Parser::parse(){
	parseProgram();
	match(tc_EOF);
	m_code->print();
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
	SymbolTableEntry* start = newLabel();
	m_code->generate(cd_GOTO, NULL, NULL, start);
	parseSubprogramDeclarations();
	m_code->generate(cd_LABEL, NULL, NULL, start);
	parseCompoundStatement();
	match(tc_DOT);
	if(m_parserError){
		TokenCode synch[] = {tc_EOF, tc_NONE};
		recover(synch);
//		if(isNext(tc_DOT)){
//			match(tc_DOT);
//		}
	}
	m_code->generate(cd_RETURN, NULL, NULL, NULL);
}

void Parser::parseIdentifierList(EntryList& idList){
	if(isNext(tc_ID)){
		idList.push_back(m_currentToken->getSymTabEntry());
	}
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
		if(isNext(tc_ID)){
			idList.push_back(m_currentToken->getSymTabEntry());
		}
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
		EntryList ids = *(new EntryList());
		parseIdentifierList(ids);
		match(tc_COLON);
		if(m_parserError){
			TokenCode synch[] = {tc_ARRAY, tc_INTEGER, tc_REAL, tc_NONE};
			recover(synch);
//			if(isNext(tc_COLON)){
//				match(tc_COLON);
//			}
		}
		parseType();
		m_code->generateVariables(ids);
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
	SymbolTableEntry* lab = newLabel();
	m_code->generate(cd_LABEL, NULL, NULL, lab);
	parseSubprogramHead();
	parseDeclarations();
	parseCompoundStatement();
	m_code->generate(cd_RETURN, NULL, NULL, NULL);
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
	EntryList ids = *(new EntryList());
	parseIdentifierList(ids);
	match(tc_COLON);
	if(m_parserError){
		TokenCode synch[] = {tc_ARRAY, tc_INTEGER, tc_REAL, tc_NONE};
		recover(synch);
//		if(isNext(tc_COLON)){
//			match(tc_COLON);
//		}
	}
	parseType();
	m_code->generateFormals(ids);
	parseParameterListPrime();
}

void Parser::parseParameterListPrime(){
	if(isNext(tc_SEMICOL)){
		match(tc_SEMICOL);
		EntryList ids = *(new EntryList());
		parseIdentifierList(ids);
		match(tc_COLON);
		if(m_parserError){
			TokenCode synch[] = {tc_ARRAY, tc_INTEGER, tc_REAL, tc_NONE};
			recover(synch);
//			if(isNext(tc_COLON)){
//				match(tc_COLON);
//			}
		}
		parseType();
		m_code->generateFormals(ids);
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
		SymbolTableEntry* exp = parseExpression();
		SymbolTableEntry* f = newLabel();
		SymbolTableEntry* end = newLabel();
		m_code->generate(cd_EQ, exp, m_symbolTable->lookup(CodeFalse), f);
		match(tc_THEN);
		if(m_parserError){
			TokenCode synch[] = {tc_ID, tc_BEGIN, tc_IF, tc_WHILE, tc_NONE};
			recover(synch);
//			if(isNext(tc_THEN)){
//				match(tc_THEN);
//			}
		}
		parseStatement();
		m_code->generate(cd_GOTO, NULL, NULL, end);
		m_code->generate(cd_LABEL, NULL, NULL, f);
		match(tc_ELSE);
		if(m_parserError){
			TokenCode synch[] = {tc_ID, tc_BEGIN, tc_IF, tc_WHILE, tc_NONE};
			recover(synch);
//			if(isNext(tc_ELSE)){
//				match(tc_ELSE);
//			}
		}
		parseStatement();
		m_code->generate(cd_LABEL, NULL, NULL, end);
	}
	else if(isNext(tc_WHILE)){
		match(tc_WHILE);
		SymbolTableEntry* start = newLabel();
		m_code->generate(cd_LABEL, NULL, NULL, start);
		SymbolTableEntry* exp = parseExpression();
		SymbolTableEntry* end = newLabel();
		m_code->generate(cd_EQ, exp, m_symbolTable->lookup(CodeFalse), end);
		match(tc_DO);
		if(m_parserError){
			TokenCode synch[] = {tc_ID, tc_BEGIN, tc_IF, tc_WHILE, tc_NONE};
			recover(synch);
//			if(isNext(tc_DO)){
//				match(tc_DO);
//			}
		}
		parseStatement();
		m_code->generate(cd_GOTO, NULL, NULL, start);
		m_code->generate(cd_LABEL, NULL, NULL, end);
	}
	else{
		parseCompoundStatement();
	}
}

void Parser::parseStatementPrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_LBRACKET) || isNext(tc_ASSIGNOP)){
		SymbolTableEntry* var = parseVariablePrime(prevEntry);
		match(tc_ASSIGNOP);
		if(m_parserError){
			TokenCode synch[] = {tc_ID, tc_NUMBER, tc_LPAREN, tc_NOT, tc_ADDOP, tc_NONE};
			recover(synch);
//			if(isNext(tc_ASSIGNOP)){
//				match(tc_ASSIGNOP);
//			}
		}
		SymbolTableEntry* exp = parseExpression();
		m_code->generate(cd_ASSIGN, exp, NULL, var);
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
		parseExpression();
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
	//expList.push_back(prevEntry);
	expList.push_back(parseExpression());
	parseExpressionListPrime(expList);
	m_code->generateCall(prevEntry, expList);
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
	SymbolTableEntry* result = prevEntry;
	if(isNext(tc_RELOP)){
		OpType t = m_currentToken->getOpType();
		match(tc_RELOP);
		SymbolTableEntry* exp = parseSimpleExpression();
		result = newTemp();
		SymbolTableEntry* tr = newLabel();
		SymbolTableEntry* end = newLabel();
		if(t == op_LT){
			m_code->generate(cd_LT, prevEntry, exp, tr);
		}
		else if(t == op_LE){
			m_code->generate(cd_LE, prevEntry, exp, tr);
		}
		else if(t == op_GT){
			m_code->generate(cd_GT, prevEntry, exp, tr);
		}
		else if(t == op_GE){
			m_code->generate(cd_GE, prevEntry, exp, tr);
		}
		else if(t == op_EQ){
			m_code->generate(cd_EQ, prevEntry, exp, tr);
		}
		else{
			m_code->generate(cd_NE, prevEntry, exp, tr);
		}
		m_code->generate(cd_ASSIGN, m_symbolTable->lookup(CodeFalse), NULL, result);
		m_code->generate(cd_GOTO, NULL, NULL, end);
		m_code->generate(cd_LABEL, NULL, NULL, tr);
		m_code->generate(cd_ASSIGN, m_symbolTable->lookup(CodeTrue), NULL, result);
		m_code->generate(cd_LABEL, NULL, NULL, end);
	}
	return result;
}

SymbolTableEntry* Parser::parseSimpleExpression(){
	int sign = false;
	if(isNext(tc_ADDOP)){
		if(m_currentToken->getOpType() == op_MINUS){
			sign = true;
		}
		parseSign();
	}
	SymbolTableEntry* entry = parseTerm();
	entry = parseSimpleExpressionPrime(entry);
	SymbolTableEntry* result = entry;
	if(sign){
		result = newTemp();
		m_code->generate(cd_UMINUS, entry, NULL, result);
	}
	return result;
}

SymbolTableEntry* Parser::parseSimpleExpressionPrime(SymbolTableEntry* prevEntry){
	SymbolTableEntry* temp = prevEntry;
	if(isNext(tc_ADDOP)){
		OpType t = m_currentToken->getOpType();
		match(tc_ADDOP);
		temp = newTemp();
		SymbolTableEntry* term = parseTerm();
		if(t == op_PLUS){
			m_code->generate(cd_ADD, prevEntry, term, temp);
		}
		else if(t == op_MINUS){
			m_code->generate(cd_SUB, prevEntry, term, temp);
		}
		else{
			m_code->generate(cd_OR, prevEntry, term, temp);
		}
		prevEntry = parseSimpleExpressionPrime(temp);
	}
	return temp;
}

SymbolTableEntry* Parser::parseTerm(){
	SymbolTableEntry* result = parseFactor();
	result = parseTermPrime(result);
	return result;
}

SymbolTableEntry* Parser::parseTermPrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_MULOP)){
		OpType t = m_currentToken->getOpType();
		match(tc_MULOP);
		SymbolTableEntry* factor = parseFactor();
		SymbolTableEntry* temp = newTemp();
		if(t == op_MULT){
			m_code->generate(cd_MULT, prevEntry, factor, temp);
		}
		else if(t == op_DIVIDE){
			m_code->generate(cd_DIVIDE, prevEntry, factor, temp);
		}
		else if(t == op_DIV){
			m_code->generate(cd_DIV, prevEntry, factor, temp);
		}
		else if(t == op_MOD){
			m_code->generate(cd_MOD, prevEntry, factor, temp);
		}
		else{
			m_code->generate(cd_AND, prevEntry, factor, temp);
		}
		prevEntry = parseTermPrime(temp);
	}
	return prevEntry;
}

SymbolTableEntry* Parser::parseFactor(){
	SymbolTableEntry* factor = NULL;
	SymbolTableEntry* result = m_symbolTable->lookup(CodeFalse);
	if(isNext(tc_ID)){
		factor = m_currentToken->getSymTabEntry();
		match(tc_ID);
		result = parseFactorPrime(factor);
	}
	else if(isNext(tc_LPAREN)){
		match(tc_LPAREN);
		result = parseExpression();
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
		factor = parseFactor();
		result = newTemp();
		m_code->generate(cd_NOT, factor, NULL, result);
	}
	else{
		result = m_currentToken->getSymTabEntry();
		match(tc_NUMBER);
		if(m_parserError){
			TokenCode synch[] = {tc_RPAREN, tc_END, tc_ADDOP, tc_MULOP, tc_RELOP, tc_THEN, tc_DO, tc_COMMA, tc_RBRACKET, tc_SEMICOL, tc_ELSE, tc_NONE};
			recover(synch);
//			if(isNext(tc_NUMBER)){
//				match(tc_NUMBER);
//			}
		}
	}
	return result;
}

SymbolTableEntry* Parser::parseFactorPrime(SymbolTableEntry* prevEntry){
	if(isNext(tc_LBRACKET)){
		parseVariablePrime(prevEntry);
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
		//m_code->generate(cd_CALL, prevEntry, NULL, NULL);
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

SymbolTableEntry* Parser::newLabel(){
	return m_symbolTable->insert(m_code->newLabel());
}

SymbolTableEntry* Parser::newTemp(){
	SymbolTableEntry* var = m_symbolTable->insert(m_code->newTemp());
	m_code->generate(cd_VAR, NULL, NULL, var);
	return var;
}
