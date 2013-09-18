%option c++
%option yylineno
%option noyywrap
%option caseless

%{
#define MaxLexemeSize 32
#include "token.h"
OpType Oper=op_NONE;
DataType Type=dt_NONE;
%}

letter	[A-Za-z]
digit	[0-9]

digits				{digit}+
int_num				{digits}
optional_fraction	("."{int_num})|""
optional_exponent	("E"("+"|"-"|""){int_num})|""
real_num			{int_num}{optional_fraction}{optional_exponent}

%%

digits				{printf("Found number: %d\n", atoi(yytext));}
int_num				{Token* t = new Token(); t->setTokenCode(tc_NUMBER); t->setDataType(dt_INTEGER); return (int)t;}
real_num			{Token* t = new Token(); t->setTokenCode(tc_NUMBER); t->setDataType(dt_REAL); return (int)t;}

%%
