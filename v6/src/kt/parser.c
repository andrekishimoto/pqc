//------------------------------------------------------------------------------
// Copyright 2024 Andre Kishimoto
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PQC - GRAMMAR
// 
// 1) <program>		::= (<stmt> | <newline>)* 
// 2) <stmt>		::= <stmt_list> <newline>
// 3) <stmt_list>	::= <let_stmt> | <reset_stmt> | <vars_stmt> | <expr_stmt>
// 4) <let_stmt>	::= "LET" <var> "=" <number>
// 5) <reset_stmt>	::= "RESET"
// 6) <vars_stmt>	::= "VARS"
// 7) <expr_stmt>	::= <expr>
// 8) <expr>		::= <term> (("+" | "-") <term>)*
// 9) <term>		::= <factor> (("*" | "/") <factor>)*
// 10) <factor>		::= <base> ("^" <factor>)*
// 11) <base>		::= "(" <expr> ")" | (<negate> <term>) | <var>
// 12) <var>		::= [A-Z]
// 13) <number>		::= <negate>? [0-9]+ ("." [0-9]+)?
// 14) <negate>		::= "~"
// 15) <newline>	::= "\n" | "\r" | "\r\n"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stdio.h>
#include "parser.h"
#include "tokenizer.h"
#include "error_type.h"
#include "utils.h"

//------------------------------------------------------------------------------
// Globals (argh!)
//------------------------------------------------------------------------------
static ktParser* g_parser = NULL;
static ktToken* lastConsumed = NULL;

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
static void reset();
static void start();
static bool advance();
static ktToken* peek(size_t ahead);
static bool consume(ktTokenType expected);

static void program();
static void stmt();
static void letStmt();
static void resetStmt();
static void varsStmt();
static void exprStmt();
static void expr();
static void term();
static void factor();
static void base();
static void var(bool evaluate);
static void number(bool evaluate);
static void negate(bool evaluate);
static void newline();

#if _DEBUG_RPN
static void rpnStmt(); 
#endif // #if _DEBUG_RPN

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
ktParser* ktParserCreate(ktParserCallback* callback)
{
	ktParser* parser = malloc(sizeof(ktParser));
	if (parser)
	{
		parser->tokenList = ktTokenListCreate();
		parser->token = NULL;
		parser->index = -1;
		parser->callback = callback;
	}

	return parser;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktParserDestroy(ktParser* parser)
{
	if (parser)
	{
		ktTokenListDestroy(parser->tokenList);
		SAFE_DELETE(parser);
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktParserRun(ktParser* parser, const char* contents)
{
	g_parser = parser;

	reset();
	ktTokenizerRun(contents, g_parser->tokenList);

#if _DEBUG_PARSER_SHOW_TOKENLIST
	ktTokenNode* curr = g_parser->tokenList->head;
	while (curr)
	{
		ktTokenPrint(curr->token);
		curr = curr->next;
	}
#endif // #if _DEBUG_PARSER_SHOW_TOKENLIST

	start();

	if (g_parser->token->type != KT_TOKEN_EOF)
	{
		// If we reached this point, then we found an error!
		if (g_parser->token->type == KT_TOKEN_WORD)
		{
			char buffer[128] = { 0 };
			snprintf(buffer, 128, ktErrorDescription(KT_ERROR_PARSER_UNKNOWN_COMMAND), g_parser->token->string);
			g_parser->callback->error(KT_ERROR_PARSER_UNKNOWN_COMMAND, buffer);
		}
		else if (g_parser->token->type == KT_TOKEN_ERROR)
		{
			g_parser->callback->error(KT_ERROR_PARSER_TOKENIZER_ERROR, g_parser->token->string);
		}
		
		// TODO: What about the other token types, such as KT_TOKEN_NUMBER, KT_TOKEN_EQUALS, etc?
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void reset()
{
	if (!g_parser)
		return;

	ktTokenListClear(g_parser->tokenList);
	g_parser->token = NULL;
	g_parser->index = -1;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void start()
{
	advance();
	program();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool advance()
{
	++g_parser->index;
	if (g_parser->index >= (int)g_parser->tokenList->count)
	{
		// If we reached this point, then we found an error!
		g_parser->callback->error(KT_ERROR_PARSER_NO_MORE_TOKENS, ktErrorDescription(KT_ERROR_PARSER_NO_MORE_TOKENS));

		return false;
	}
	else
	{
		if (g_parser->index > 0)
		{
			ktTokenListCursorAdvance(g_parser->tokenList);
		}
		g_parser->token = g_parser->tokenList->cursor->token;
		
		return true;
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
ktToken* peek(size_t ahead)
{
	if (!g_parser)
		return NULL;

	if (g_parser->index + ahead < g_parser->tokenList->count)
	{
		ktTokenNode* originalCursor = g_parser->tokenList->cursor;
		for (size_t i = 0; i < ahead; ++i)
		{
			ktTokenListCursorAdvance(g_parser->tokenList);
		}
		ktToken* token = g_parser->tokenList->cursor->token;
		g_parser->tokenList->cursor = originalCursor;

		return token;
	}

	return NULL;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool consume(ktTokenType expected)
{
	if (g_parser->token->type == expected)
	{
		lastConsumed = g_parser->token;
		advance();
		
		return true;
	}
	else
	{
		// If we reached this point, then we found an error!
		char buffer[128] = { 0 };
		snprintf(buffer, 128, ktErrorDescription(KT_ERROR_PARSER_CONSUME_EXPECTED_GOT), KT_TOKEN_TYPE_STR[expected], KT_TOKEN_TYPE_STR[g_parser->token->type]);
		g_parser->callback->error(KT_ERROR_PARSER_CONSUME_EXPECTED_GOT, buffer);

		// HACK: Since there is an error, let's skip right to the end of the token list.
		size_t steps = (g_parser->tokenList->count - 1) - g_parser->index;
		while (steps)
		{
			++g_parser->index;
			ktTokenListCursorAdvance(g_parser->tokenList);
			--steps;
		}
		g_parser->token = g_parser->tokenList->cursor->token;

		return false;
	}
}

//------------------------------------------------------------------------------
// 1) <program>		::= (<stmt> | <newline>)* 
//------------------------------------------------------------------------------
void program()
{
	DEBUG_PRINT("[parser] program()\n");

	while (g_parser->token->type == KT_TOKEN_STMT_LET
		|| g_parser->token->type == KT_TOKEN_STMT_RESET
		|| g_parser->token->type == KT_TOKEN_STMT_VARS
		|| g_parser->token->type == KT_TOKEN_OPEN_PAREN
		|| g_parser->token->type == KT_TOKEN_VAR
		|| g_parser->token->type == KT_TOKEN_NEG
		|| g_parser->token->type == KT_TOKEN_NEWLINE
#if _DEBUG_RPN
		|| g_parser->token->type == KT_TOKEN_STMT_RPN
#endif // #if _DEBUG_RPN
	)
	{
		if (g_parser->token->type == KT_TOKEN_STMT_LET
			|| g_parser->token->type == KT_TOKEN_STMT_RESET
			|| g_parser->token->type == KT_TOKEN_STMT_VARS
			|| g_parser->token->type == KT_TOKEN_OPEN_PAREN
			|| g_parser->token->type == KT_TOKEN_VAR
			|| g_parser->token->type == KT_TOKEN_NEG
#if _DEBUG_RPN
			|| g_parser->token->type == KT_TOKEN_STMT_RPN
#endif // #if _DEBUG_RPN
		)
		{
			stmt();
		}

		if (g_parser->token->type == KT_TOKEN_NEWLINE)
		{
			advance();
		}
	}
}

//------------------------------------------------------------------------------
// 2) <stmt>		::= <stmt_list> <newline>
// 3) <stmt_list>	::= <let_stmt> | <reset_stmt> | <vars_stmt> | <expr_stmt>
//------------------------------------------------------------------------------
void stmt()
{
	DEBUG_PRINT("[parser] stmt()\n");

	if (g_parser->token->type == KT_TOKEN_STMT_LET)
	{
		letStmt();
	}
	else if (g_parser->token->type == KT_TOKEN_STMT_RESET)
	{
		resetStmt();
	}
	else if (g_parser->token->type == KT_TOKEN_STMT_VARS)
	{
		varsStmt();
	}
#if _DEBUG_RPN
	else if (g_parser->token->type == KT_TOKEN_STMT_RPN)
	{
		rpnStmt();
	}
#endif // #if _DEBUG_RPN
	else
	{
		exprStmt();
	}
}

//------------------------------------------------------------------------------
// 4) <let_stmt>	::= "LET" <var> "=" <number>
//------------------------------------------------------------------------------
void letStmt()
{
	DEBUG_PRINT("[parser] letStmt()\n");

	DEBUG_PRINT("[parser] consume(KT_TOKEN_STMT_LET)\n");
	consume(KT_TOKEN_STMT_LET);

	var(false);
	bool variableConsumed = (lastConsumed && lastConsumed->type == KT_TOKEN_VAR);
	char variable = variableConsumed ? lastConsumed->var : '\0';

	DEBUG_PRINT("[parser] consume(KT_TOKEN_EQUALS)\n");
	bool equalsConsumed = consume(KT_TOKEN_EQUALS);
	int errorCode = (equalsConsumed ? 0 : 1);
	g_parser->callback->symbol(errorCode, lastConsumed ? lastConsumed->symbol : '\0');

	number(false);
	bool numberConsumed = (lastConsumed && lastConsumed->type == KT_TOKEN_NUMBER);
	double number = numberConsumed ? lastConsumed->number : 0.0;

	newline();
	bool newlineConsumed = (lastConsumed && lastConsumed->type == KT_TOKEN_NEWLINE);
	
	errorCode = 0;
	if (!variableConsumed) errorCode |= 0x01;
	if (!numberConsumed) errorCode |= 0x02;
	if (!errorCode && !newlineConsumed) errorCode |= 0x04;
	g_parser->callback->letStmt(errorCode, variable, number);
}

//------------------------------------------------------------------------------
// 5) <reset_stmt>	::= "RESET"
//------------------------------------------------------------------------------
void resetStmt()
{
	DEBUG_PRINT("[parser] resetStmt()\n");

	DEBUG_PRINT("[parser] consume(KT_TOKEN_STMT_RESET)\n");
	bool resetConsumed = consume(KT_TOKEN_STMT_RESET);

	newline();
	bool newlineConsumed = (lastConsumed && lastConsumed->type == KT_TOKEN_NEWLINE);
	int errorCode = (resetConsumed && newlineConsumed ? 0 : 1);

	g_parser->callback->resetStmt(errorCode);
}

//------------------------------------------------------------------------------
// 6) <vars_stmt>	::= "VARS"
//------------------------------------------------------------------------------
void varsStmt()
{
	DEBUG_PRINT("[parser] varsStmt()\n");

	DEBUG_PRINT("[parser] consume(KT_TOKEN_STMT_VARS)\n");
	bool varsConsumed = consume(KT_TOKEN_STMT_VARS);

	newline();
	bool newlineConsumed = (lastConsumed && lastConsumed->type == KT_TOKEN_NEWLINE);
	int errorCode = (varsConsumed && newlineConsumed ? 0 : 1);

	g_parser->callback->varsStmt(errorCode);
}

//------------------------------------------------------------------------------
// 7) <expr_stmt>	::= <expr>
//------------------------------------------------------------------------------
void exprStmt()
{
	DEBUG_PRINT("[parser] exprStmt()\n");

	g_parser->callback->exprStmtBegin(0);

	expr();

	// We know an <expr_stmt> reached its end when we find a line break.
	newline();
	bool newlineConsumed = (lastConsumed && lastConsumed->type == KT_TOKEN_NEWLINE);
	int errorCode = (newlineConsumed ? 0 : 1);

	g_parser->callback->exprStmtEnd(errorCode);
}

//------------------------------------------------------------------------------
// 8) <expr>		::= <term> (("+" | "-") <term>)*
//------------------------------------------------------------------------------
void expr()
{
	DEBUG_PRINT("[parser] expr()\n");

	term();
	
	while (g_parser->token->type == KT_TOKEN_ADD || g_parser->token->type == KT_TOKEN_SUB)
	{
		if (g_parser->token->type == KT_TOKEN_ADD)
		{
			DEBUG_PRINT("[parser] consume(KT_TOKEN_ADD)\n");
			bool addConsumed = consume(KT_TOKEN_ADD);
			int errorCode = (addConsumed ? 0 : 1);
			g_parser->callback->symbol(errorCode, lastConsumed->symbol);
		}
		else if (g_parser->token->type == KT_TOKEN_SUB)
		{
			DEBUG_PRINT("[parser] consume(KT_TOKEN_SUB)\n");
			bool subConsumed = consume(KT_TOKEN_SUB);
			int errorCode = (subConsumed ? 0 : 1);
			g_parser->callback->symbol(errorCode, lastConsumed->symbol);
		}

		term();
	}
}

//------------------------------------------------------------------------------
// 9) <term>		::= <factor> (("*" | "/") <factor>)*
//------------------------------------------------------------------------------
void term()
{
	DEBUG_PRINT("[parser] term()\n");

	factor();

	while (g_parser->token->type == KT_TOKEN_MUL || g_parser->token->type == KT_TOKEN_DIV)
	{
		if (g_parser->token->type == KT_TOKEN_MUL)
		{
			DEBUG_PRINT("[parser] consume(KT_TOKEN_MUL)\n");
			bool mulConsumed = consume(KT_TOKEN_MUL);
			int errorCode = (mulConsumed ? 0 : 1);
			g_parser->callback->symbol(errorCode, lastConsumed->symbol);
		}
		else if (g_parser->token->type == KT_TOKEN_DIV)
		{
			DEBUG_PRINT("[parser] consume(KT_TOKEN_DIV)\n");
			bool divConsumed = consume(KT_TOKEN_DIV);
			int errorCode = (divConsumed ? 0 : 1);
			g_parser->callback->symbol(errorCode, lastConsumed->symbol);
		}

		factor();
	}
}

//------------------------------------------------------------------------------
// 10) <factor>		::= <base> ("^" <factor>)*
//------------------------------------------------------------------------------
void factor()
{
	DEBUG_PRINT("[parser] factor()\n");

	base();

	while (g_parser->token->type == KT_TOKEN_POW)
	{
		DEBUG_PRINT("[parser] consume(KT_TOKEN_POW)\n");
		bool powConsumed = consume(KT_TOKEN_POW);
		int errorCode = (powConsumed ? 0 : 1);
		g_parser->callback->symbol(errorCode, lastConsumed->symbol);

		factor();
	}
}

//------------------------------------------------------------------------------
// 11) <base>		::= "(" <expr> ")" | (<negate> <term>) | <var>
//------------------------------------------------------------------------------
void base()
{
	DEBUG_PRINT("[parser] base()\n");

	if (g_parser->token->type == KT_TOKEN_OPEN_PAREN)
	{
		DEBUG_PRINT("[parser] consume(KT_TOKEN_OPEN_PAREN)\n");
		bool openParenConsumed = consume(KT_TOKEN_OPEN_PAREN);
		int errorCode = (openParenConsumed ? 0 : 1);
		g_parser->callback->symbol(errorCode, lastConsumed->symbol);

		expr();

		DEBUG_PRINT("[parser] consume(KT_TOKEN_CLOSE_PAREN)\n");
		bool closeParenConsumed = consume(KT_TOKEN_CLOSE_PAREN);
		errorCode = (closeParenConsumed ? 0 : 1);
		g_parser->callback->symbol(errorCode, lastConsumed->symbol);
	}
	else if (g_parser->token->type == KT_TOKEN_NEG)
	{
		negate(true);
		term();
	}
	else if (g_parser->token->type == KT_TOKEN_VAR)
	{
		var(true);
	}
}

//------------------------------------------------------------------------------
// 12) <var>		::= [A-Z]
//------------------------------------------------------------------------------
void var(bool evaluate)
{
	DEBUG_PRINT("[parser] var()\n");

	DEBUG_PRINT("[parser] consume(KT_TOKEN_VAR) = %c\n", g_parser->token->var);
	bool varConsumed = consume(KT_TOKEN_VAR);

	if (evaluate)
	{
		int errorCode = (varConsumed ? 0 : 1);
		g_parser->callback->var(errorCode, lastConsumed->var);
	}
}

//------------------------------------------------------------------------------
// 13) <number>		::= <negate>? [0-9]+ ("." [0-9]+)?
//------------------------------------------------------------------------------
void number(bool evaluate)
{
	DEBUG_PRINT("[parser] number()\n");

	bool isNegative = (g_parser->token->type == KT_TOKEN_NEG);
	if (isNegative)
	{
		negate(evaluate);
	}

	double number = g_parser->token->number;
	if (isNegative)
	{
		number = -number;
	}

	DEBUG_PRINT("[parser] consume(KT_TOKEN_NUMBER) = %f\n", number);
	bool numberConsumed = consume(KT_TOKEN_NUMBER);

	if (numberConsumed)
	{
		lastConsumed->number = number;
	}

	if (evaluate)
	{
		int errorCode = (numberConsumed ? 0 : 1);
		g_parser->callback->number(errorCode, lastConsumed->number);
	}
}

//------------------------------------------------------------------------------
// 14) <negate>		::= "~"
//------------------------------------------------------------------------------
void negate(bool evaluate)
{
	DEBUG_PRINT("[parser] negate()\n");

	DEBUG_PRINT("[parser] consume(KT_TOKEN_NEG)\n");
	bool negConsumed = consume(KT_TOKEN_NEG);
	
	if (evaluate)
	{
		int errorCode = (negConsumed ? 0 : 1);
		g_parser->callback->symbol(errorCode, lastConsumed->symbol);
	}
}

//------------------------------------------------------------------------------
// 15) <newline>	::= "\n" | "\r" | "\r\n"
//------------------------------------------------------------------------------
void newline()
{
	DEBUG_PRINT("[parser] newline()\n");

	DEBUG_PRINT("[parser] consume(KT_TOKEN_NEWLINE)\n");
	bool newlineConsumed = consume(KT_TOKEN_NEWLINE);
	int errorCode = (newlineConsumed ? 0 : 1);
	g_parser->callback->symbol(errorCode, lastConsumed->symbol);
}

#if _DEBUG_RPN
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void rpnStmt()
{
	DEBUG_PRINT("[parser] consume(KT_TOKEN_STMT_RPN)\n");
	bool rpnConsumed = consume(KT_TOKEN_STMT_RPN);
	int errorCode = (rpnConsumed ? 0 : 1);
	g_parser->callback->rpnStmt(errorCode);
}
#endif // #if _DEBUG_RPN
