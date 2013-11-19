// **************************************************************
// *                                                            *
// *    C O D E     (Header)                                    *
// *                                                            *
// *    CLASSES:    Quadruple, QuadrupleList, Code              *
// *                                                            *
// **************************************************************

#ifndef	code_h
#define	code_h

#include <stdlib.h>
#include <sstream>  // So you can std::stringstream to generate labels and temp vars
#include "symtab.h"

class Quadruple {				/* A record for intermediate code  */
private:
	CodeOp m_op;				/* operator */
	SymbolTableEntry* m_arg1;	/* arg1 is in symbol table */
	SymbolTableEntry* m_arg2;	/* arg2 is in symbol table */
	SymbolTableEntry* m_result;	/* result is in symbol table */
public:
	Quadruple();
	Quadruple(CodeOp op, SymbolTableEntry* arg1, SymbolTableEntry* arg2, SymbolTableEntry* result);
	CodeOp getOp();
	void set(CodeOp op, SymbolTableEntry* arg1, SymbolTableEntry* arg2, SymbolTableEntry* result);
	
	// Use this function to print out the quadruples
	static void print(Quadruple& quad)
	{
		static bool lastisLabel = false;
		std::string opStr = CodeOpToString(quad.m_op);
		if(quad.m_op == cd_LABEL)
		{
			if (lastisLabel)
				printf("\n"); 
			printf("%7s", quad.m_result->lexeme.c_str());
			printf("%c",':');
			lastisLabel = true;
		}
		else
		{
			if(!lastisLabel)
				printf("%8s"," ");
			printf("%10s",opStr.c_str());
			if(quad.m_arg1 != NULL)
				printf("%15s", quad.m_arg1->lexeme.c_str());
			else
				printf("%15s"," ");
			if(quad.m_arg2 != NULL)
				printf("%15s", quad.m_arg2->lexeme.c_str());
			else
				printf("%15s"," ");
			if (quad.m_result != NULL)
				printf("%15s\n", quad.m_result->lexeme.c_str());
			else
				printf("%15s\n"," ");
			lastisLabel = false;
		}
	};
};

typedef std::vector<Quadruple> QuadrupleList;

// Use this constants for True/False code constants
static const std::string CodeFalse = "0";
static const std::string CodeTrue = "1";

// Use this constant when generating new labels
static const std::string CodeLabelPrefix = "lab";
// Use this constant when generating new temporary variable names
static const std::string CodeTempVarPrefix = "t";	

class Code {
private:
	QuadrupleList m_qList;
	int m_tempCounter;
	int m_labelCounter;
public:
	Code();
	~Code();
	void generate(CodeOp op, SymbolTableEntry* arg1, SymbolTableEntry* arg2, SymbolTableEntry* result);
	void generateCall(SymbolTableEntry* entry, EntryList& eList);
	void generateFormals(EntryList& entrylist);
	void generateVariables(EntryList& entrylist);
	std::string newLabel();
	std::string newTemp();
	void print();
};

#endif
