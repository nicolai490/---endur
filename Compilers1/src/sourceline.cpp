/*
 * sourceline.cpp
 *
 *  Created on: Oct 14, 2013
 *      Author: ubuntu
 */

#include <iostream>
#include "sourceline.h"

int LineError::getColumn(){
	return m_column;
}

std::string LineError::getError(){
	return m_errorMsg;
}

void LineError::set(int col, const std::string& err){
	m_column = col;
	m_errorMsg = err;
}

SourceLine::SourceLine(){
	m_line = *(new std::string());
	m_lineNo = 1;
	m_firstLexemeColumn = 0;
	m_lastLexemeColumn = 0;
	m_errors = *(new std::vector<LineError>());
}

void SourceLine::buildLine(const std::string& str){
	m_line += str;
	m_firstLexemeColumn = m_lastLexemeColumn + 1;
	m_lastLexemeColumn += str.size();
}

void SourceLine::addError(const std::string& msg){
	if(m_errors.size() >= MAX_ERRORS_IN_LINE) return;
	LineError error = *(new LineError());
	error.set(m_firstLexemeColumn, msg);
	m_errors.push_back(error);
}

bool SourceLine::hasError(){
	return ! m_errors.empty();
}

void SourceLine::printLine(){
	std::cout << m_lineNo << " : " << m_line << "\n";
	for(int i = 0; i < (int)m_errors.size(); i++){
		LineError error = m_errors[i];
		
		//Length of the line number:
		int num = m_lineNo;
		while(num > 0){
			std::cout << " ";
			num /= 10;
		}
		
		std::cout << "  ";
		
		num = error.getColumn();
		while(num > 0){
			std::cout << " ";
			num--;
		}
		
		std::cout << "^ " << error.getError() << "\n";
	}
}

void SourceLine::newLine(){
	m_line.clear();
	m_errors.clear();
	m_firstLexemeColumn = 0;
	m_lastLexemeColumn = 0;
	m_lineNo++;
}
