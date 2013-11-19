// **************************************************************
// *                                                            *
// *    P A R S E R    (Header)                                 *
// *                                                            *
// *    CLASSES:       Parser                                   *
// *                                                            *
// **************************************************************

#ifndef	parser_h
#define	parser_h

#include "scanner.h"

/* Code Generation includes, uncomment in part 3 */
#include "code.h"

class Parser {

private:
	Scanner* m_lexan;			// Lexical analyzer
	SymbolTable* m_symbolTable;	// Symbol table
	Token* m_currentToken;		// Current token
	bool m_parserError;			// Has a parsing error occurred?
	int  m_totalErrors;			// Total number of errors

	bool tokenCodeIn(TokenCode tc, const TokenCode list[]);	// Is the given tokencode in the syncronizing set
	void recover(const TokenCode list[]);	// keep asking for tokens until token is found in the synchronizing set
	void getToken();						// ask lexical analyzer for next token
	void match(TokenCode tc);				// Walk over tokencode tc.  If not a match; report an error
	void setError(const std::string& err);	// sets an appropriate error message
	void expectedTokenCode(TokenCode tc);	// adds an error regarding an expected token that was not found
	TokenCode getTokenCode();				// get the token code of the current token
	bool isNext(TokenCode tc);				// check if a certain token code is next in the input
	
	void parseProgram();
	void parseIdentifierList(EntryList& idList);
	void parseIdentifierListPrime(EntryList& idList);
 	void parseDeclarations();
	void parseType();
	void parseStandardType();
	void parseSubprogramDeclarations();
	void parseSubprogramDeclaration();
	void parseSubprogramHead();
	void parseArguments();
	void parseParameterList();
	void parseParameterListPrime();
	void parseCompoundStatement();
	void parseOptionalStatements();
	void parseStatementList();
	void parseStatementListPrime();
	void parseStatement();
	void parseStatementPrime(SymbolTableEntry* prevEntry);
	SymbolTableEntry* parseVariable();
	SymbolTableEntry* parseVariablePrime(SymbolTableEntry* prevEntry);
	void parseProcedureStatement();
	void parseProcedureStatementPrime(SymbolTableEntry* prevEntry);
	void parseExpressionList(SymbolTableEntry* prevEntry);
	void parseExpressionListPrime(EntryList& expList);
	SymbolTableEntry* parseExpression();
	SymbolTableEntry* parseExpressionPrime(SymbolTableEntry* prevEntry);
	SymbolTableEntry* parseSimpleExpression();
	SymbolTableEntry* parseSimpleExpressionPrime(SymbolTableEntry* prevEntry);
	SymbolTableEntry* parseTerm();
	SymbolTableEntry* parseTermPrime(SymbolTableEntry* prevEntry);
	SymbolTableEntry* parseFactor();
	SymbolTableEntry* parseFactorPrime(SymbolTableEntry* prevEntry);
	void parseSign();
	
public:
	Parser(std::istream& input, std::ostream& output);
	~Parser();
	void parse();
	SymbolTable* getSymbolTable();  
	int totalErrors();
	
/* Code generation, implement in part 3, uncomment then */
private:
	Code*  m_code;			// The Target Code
	
	SymbolTableEntry* newLabel();	// Generate a new symbol table entry for a new code label
	SymbolTableEntry* newTemp();	// Generate a new symbol table entry for a new temporary variable
	CodeOp opToCode(OpType op);	// Get the corresponding operator in the target code based on the operator type
public:
	Code* getCode();	// Getter for the target code

};

#endif
