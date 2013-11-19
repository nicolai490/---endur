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
	
}
