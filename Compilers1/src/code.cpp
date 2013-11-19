/*
 * code.cpp
 *
 *  Created on: Nov 19, 2013
 *      Author: ubuntu
 */

#include "code.h"

Quadruple::Quadruple(){
	m_op = cd_NOOP;
}

Quadruple::Quadruple(enum CodeOp op, SymbolTableEntry* arg1, SymbolTableEntry* arg2, SymbolTableEntry* result){
	m_op = op;
	m_arg1 = arg1;
	m_arg2 = arg2;
	m_result = result;
}

CodeOp Quadruple::getOp(){
	return m_op;
}

void Quadruple::set(enum CodeOp op, SymbolTableEntry* arg1, SymbolTableEntry* arg2, SymbolTableEntry* result){
	m_op = op;
	m_arg1 = arg1;
	m_arg2 = arg2;
	m_result = result;
}

Code::Code(){
	m_qList = *(new QuadrupleList());
	m_tempCounter = 0;
	m_labelCounter = 0;
}

Code::~Code(){
	
}

void Code::generate(enum CodeOp op, SymbolTableEntry* arg1, SymbolTableEntry* arg2, SymbolTableEntry* result){
	m_qList.push_back(*(new Quadruple(op, arg1, arg2, result)));
}

std::string Code::newLabel(){
	std::stringstream s;
	s << CodeLabelPrefix << m_labelCounter++;
	return s.str();
}

std::string Code::newTemp(){
	std::stringstream s;
	s << CodeTempVarPrefix << m_tempCounter++;
	return s.str();
}
