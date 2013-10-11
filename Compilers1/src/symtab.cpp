/*
 * symtab.cpp
 *
 *  Created on: Sep 19, 2013
 *      Author: ubuntu
 */

#include "symtab.h"

SymbolTable::SymbolTable(void){
    m_table = *(new EntryList());
}

SymbolTable::~SymbolTable(void){
	for(int i = 0; i < (int)m_table.size(); i++){
		delete m_table[i];
	}
}

SymbolTableEntry* SymbolTable::insert(const std::string lexeme){
	std::string lex = *(new std::string());
	for(int i = 0; i < (int)lexeme.size(); i++){
		lex.push_back(std::tolower(lexeme[i]));
	}
	SymbolTableEntry* entry = lookup(lex);
	if(entry == NULL){
		entry = new SymbolTableEntry();
		entry->lexeme = lex;
		m_table.push_back(entry);
	}
	return entry;
}

SymbolTableEntry* SymbolTable::lookup(const std::string& lexeme){
	for(int i = 0; i < (int)m_table.size(); i++){
		if(lexeme.compare(m_table[i]->lexeme) == 0){
			return m_table[i];
		}
	}
	return NULL;
}
