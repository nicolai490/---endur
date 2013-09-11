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

%option main

letter  [A-Za-z]
digit   [0-9]
id      {letter}({letter}|{digit})*

digits              {digit}+
int_num             {digits}
optional_fraction   ("."{int_num})|""
optional_exponent   ("E"("+"|"-"|""){int_num})|""
real_num            {int_num}{optional_fraction}{optional_exponent}

%%

.       {printf("Just a test");}

%%

int yywrap(void){return 1;}
