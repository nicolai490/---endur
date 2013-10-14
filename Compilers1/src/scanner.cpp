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
	m_currentToken = *(new Token());
	m_sourceLine = *(new SourceLine());
}

Scanner::~Scanner(void){
	delete m_lexer;
}

SymbolTable* Scanner::getSymbolTable(void){
	return m_symbolTable;
}

void Scanner::setCurrentToken(TokenCode tc, DataType dt, OpType op){
	//printf("Creating token %d %d %d\n", tc, dt, op);
	//m_currentToken = *(new Token());
	m_currentToken.setTokenCode(tc);
	m_currentToken.setDataType(dt);
	m_currentToken.setOpType(op);
}

void Scanner::setCurrentToken(TokenCode tc, DataType dt, const std::string& lexeme){
	//printf("Creating token %d %d 5\n", tc, dt);
	//m_currentToken = *(new Token());
	m_currentToken.setTokenCode(tc);
	m_currentToken.setDataType(dt);
	m_currentToken.setOpType(op_NONE);
	SymbolTableEntry* entry = m_symbolTable->insert(lexeme);
	m_currentToken.setSymTabEntry(entry);
}

Token* Scanner::nextToken(void){
	TokenCode tc = (TokenCode)m_lexer->yylex();
	while(tc == tc_SPACE || tc == tc_TAB || tc == tc_NEWLINE || tc == tc_COMMENT){
		if(tc == tc_NEWLINE){
			flushSourceLine();
		}
		else{
			m_sourceLine.buildLine(m_lexer->YYText());
		}
		tc = (TokenCode)m_lexer->yylex();
	}
	if(tc == tc_ID || tc == tc_NUMBER){
		setCurrentToken(tc, Type, m_lexer->YYText());
	}
	else{
		setCurrentToken(tc, Type, Oper);
	}
	m_sourceLine.buildLine(m_lexer->YYText());
	return &m_currentToken;
}

void Scanner::flushSourceLine(){
	if(m_sourceLine.hasError()){
		m_sourceLine.printLine();
	}
	m_sourceLine.newLine();
}

void Scanner::addError(std::string error){
	m_sourceLine.addError(error);
}
