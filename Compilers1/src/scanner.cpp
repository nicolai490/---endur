/*
 * scanner.cpp
 *
 *  Created on: Sep 19, 2013
 *      Author: ubuntu
 */

#include "scanner.h"
#include "lex.yy.cc"

Scanner::Scanner(SymbolTable* symbols, std::istream& input, std::ostream& output){
	m_symbolTable = symbols;
	m_lexer = new yyFlexLexer(&input, &output);
}

Scanner::~Scanner(void){
	m_lexer->~FlexLexer();
}

SymbolTable* Scanner::getSymbolTable(void){
	return m_symbolTable;
}

void Scanner::setCurrentToken(TokenCode tc, DataType dt, OpType op){
	m_currentToken = *(new Token());
	m_currentToken.setTokenCode(tc);
	m_currentToken.setDataType(dt);
	m_currentToken.setOpType(op);
}

void Scanner::setCurrentToken(TokenCode tc, DataType dt, const std::string& lexeme){
	m_currentToken = *(new Token());
	m_currentToken.setTokenCode(tc);
	m_currentToken.setDataType(dt);
	m_currentToken.setOpType(op_NONE);
	SymbolTableEntry* entry = m_symbolTable->insert(lexeme);
	m_currentToken.setSymTabEntry(entry);
}

Token* Scanner::nextToken(void){
	TokenCode tc = (TokenCode)m_lexer->yylex();
	if(tc == tc_ID || tc == tc_NUMBER){
		setCurrentToken(tc, Type, m_lexer->YYText());
	}
	else{
		setCurrentToken(tc, Type, Oper);
	}
	return &m_currentToken;
}
