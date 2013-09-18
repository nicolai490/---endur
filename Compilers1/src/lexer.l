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
id      {letter}({letter}|{digit})*

digits              {digit}+
int_num             {digits}
optional_fraction   ("."{int_num})|""
optional_exponent   ("E"("+"|"-"|""){int_num})|""
real_num            {int_num}{optional_fraction}{optional_exponent}
relop               "="|"<"|"<="|">"|">="|"<>"
addop               "+"|"-"|"or"
mulop               "*"|"/"|"and"|"div"|"mod"
assignop            ":="

%%

{int_num}	{Oper = op_NONE; Type = dt_INTEGER; return tc_NUMBER;}
{real_num}	{Oper = op_NONE; Type = dt_REAL; return tc_NUMBER;}

"program"	{Oper = op_NONE; Type = dt_KEYWORD; return tc_PROGRAM;}
"var"		{Oper = op_NONE; Type = dt_KEYWORD; return tc_VAR;}
"array"		{Oper = op_NONE; Type = dt_KEYWORD; return tc_ARRAY;}
"of"		{Oper = op_NONE; Type = dt_KEYWORD; return tc_OF;}
"integer"	{Oper = op_NONE; Type = dt_KEYWORD; return tc_INTEGER;}
"real"		{Oper = op_NONE; Type = dt_KEYWORD; return tc_REAL;}
"function"	{Oper = op_NONE; Type = dt_KEYWORD; return tc_FUNCTION;}
"procedure"	{Oper = op_NONE; Type = dt_KEYWORD; return tc_PROCEDURE;}
"begin"		{Oper = op_NONE; Type = dt_KEYWORD; return tc_BEGIN;}
"end"		{Oper = op_NONE; Type = dt_KEYWORD; return tc_END;}
"if"		{Oper = op_NONE; Type = dt_KEYWORD; return tc_IF;}
"then"		{Oper = op_NONE; Type = dt_KEYWORD; return tc_THEN;}
"else"		{Oper = op_NONE; Type = dt_KEYWORD; return tc_ELSE;}
"while"		{Oper = op_NONE; Type = dt_KEYWORD; return tc_WHILE;}
"do"		{Oper = op_NONE; Type = dt_KEYWORD; return tc_DO;}
"not"		{Oper = op_NONE; Type = dt_KEYWORD; return tc_NOT;}

" "			{Oper = op_NONE; Type = dt_NONE; return tc_SPACE;}
"\t"		{Oper = op_NONE; Type = dt_NONE; return tc_TAB;}
"\n"		{Oper = op_NONE; Type = dt_NONE; return tc_NEWLINE;}
"{"[^}]*"}"	{Oper = op_NONE; Type = dt_NONE; return tc_COMMENT;}

%%
