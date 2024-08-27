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
#include "utils.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define PARSER_DEBUG_OUTPUT 0
#if PARSER_DEBUG_OUTPUT
#define DEBUG_PRINT(...)	printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)	
#endif

//------------------------------------------------------------------------------
// Globals (argh!)
//------------------------------------------------------------------------------
static Parser* g_parser = NULL;
static Token* lastConsumed = NULL;
static bool hasError = false;

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
static void reset();
static void start();
static bool advance();
static Token* peek(size_t ahead);
static bool consume(TokenType expected);

static void program();
static void stmt();
static void letStmt();
static void resetStmt();
static void varsStmt();
static void exprStmt();
static void rpnStmt(); // TODO: *** DEBUG CODE - REMOVE ME FROM FINAL BUILD. ***
static void expr();
static void term();
static void factor();
static void base();
static void var(bool evaluate);
static void number(bool evaluate);
static void negate();
static void newline();

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Parser* ktParserCreate(ParserCallback* callback)
{
	Parser* parser = malloc(sizeof(Parser));
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
void ktParserDestroy(Parser* parser)
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
void ktParserRun(Parser* parser, const char* contents)
{
	g_parser = parser;

	reset();
	ktTokenizerRun(contents, g_parser->tokenList);

#if PARSER_DEBUG_OUTPUT
	TokenNode* curr = g_parser->tokenList->head;
	while (curr)
	{
		ktTokenPrint(curr->token);
		curr = curr->next;
	}
#endif // #if PARSER_DEBUG_OUTPUT

	start();

	if (g_parser->token->type != TOKEN_EOF)
	{
		// TODO: If we landed here, then it is an error!
		// Expected EOF.

		//g_parser->callback->error(3, "Expected TOKEN_EOF.");

		if (g_parser->token->type == TOKEN_WORD)
		{
			char buffer[128] = { 0 };
			snprintf(buffer, 128, "Unknown command '%s'.", g_parser->token->string);
			g_parser->callback->error(3, buffer);
		}
		else if (g_parser->token->type == TOKEN_ERROR)
		{
			g_parser->callback->error(4, g_parser->token->string);
		}
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
		// TODO: If we landed here, then it is an error!
		// There are no more tokens to parse (unexpected EOF).
		g_parser->callback->error(2, "There are no more tokens to parse (unexpected TOKEN_EOF).");

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
Token* peek(size_t ahead)
{
	if (!g_parser)
		return NULL;

	if (g_parser->index + ahead < g_parser->tokenList->count)
	{
		TokenNode* originalCursor = g_parser->tokenList->cursor;
		for (size_t i = 0; i < ahead; ++i)
		{
			ktTokenListCursorAdvance(g_parser->tokenList);
		}
		Token* token = g_parser->tokenList->cursor->token;
		g_parser->tokenList->cursor = originalCursor;

		return token;
	}

	return NULL;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool consume(TokenType expected)
{
	if (g_parser->token->type == expected)
	{
		lastConsumed = g_parser->token;
		advance();
		
		return true;
	}
	else
	{
		// TODO: If we landed here, then it is an error!
		// Incorrect token - Expected: expected. Got: g_parser->token->type.
		char buffer[128] = { 0 };
		snprintf(buffer, 128, "Incorrect token. Expected: %s, got: %s.", TOKEN_TYPE_STR[expected], TOKEN_TYPE_STR[g_parser->token->type]);
		g_parser->callback->error(1, buffer);

		// HACK: If there's an error, let's skip right to the end of the token list.
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

	while (g_parser->token->type == TOKEN_STMT_LET
		|| g_parser->token->type == TOKEN_STMT_RESET
		|| g_parser->token->type == TOKEN_STMT_VARS
		|| g_parser->token->type == TOKEN_STMT_RPN // TODO: *** DEBUG CODE - REMOVE ME FROM FINAL BUILD. ***
		|| g_parser->token->type == TOKEN_OPEN_PAREN
		|| g_parser->token->type == TOKEN_VAR
		|| g_parser->token->type == TOKEN_NEG
		|| g_parser->token->type == TOKEN_NEWLINE)
	{
		if (g_parser->token->type == TOKEN_STMT_LET
			|| g_parser->token->type == TOKEN_STMT_RESET
			|| g_parser->token->type == TOKEN_STMT_VARS
			|| g_parser->token->type == TOKEN_STMT_RPN // TODO: *** DEBUG CODE - REMOVE ME FROM FINAL BUILD. ***
			|| g_parser->token->type == TOKEN_OPEN_PAREN
			|| g_parser->token->type == TOKEN_VAR
			|| g_parser->token->type == TOKEN_NEG)
		{
			stmt();
		}

		if (g_parser->token->type == TOKEN_NEWLINE)
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

	hasError = false;

	if (g_parser->token->type == TOKEN_STMT_LET)
	{
		letStmt();
	}
	else if (g_parser->token->type == TOKEN_STMT_RESET)
	{
		resetStmt();
	}
	else if (g_parser->token->type == TOKEN_STMT_VARS)
	{
		varsStmt();
	}
	else if (g_parser->token->type == TOKEN_STMT_RPN) // TODO: *** DEBUG CODE - REMOVE ME FROM FINAL BUILD. ***
	{
		rpnStmt();
	}
	else
	{
		exprStmt();
	}

	// HACK: If any of the statements above was parsed without errors,
	// jump right to the next TOKEN_NEWLINE. This will skip all tokens
	// that came after a statement.
	// E.g.: RESET XYZ
	// The RESET statement is parsed correctly and we ignore XYZ.
	if (!hasError)
	{
		while (g_parser->token->type != TOKEN_NEWLINE)
		{
			advance();
		}
	}

	newline();
}

//------------------------------------------------------------------------------
// 4) <let_stmt>	::= "LET" <var> "=" <number>
//------------------------------------------------------------------------------
void letStmt()
{
	DEBUG_PRINT("[parser] letStmt()\n");

	DEBUG_PRINT("[parser] consume(TOKEN_STMT_LET)\n");
	consume(TOKEN_STMT_LET);

	var(false);
	bool variableConsumed = (lastConsumed && lastConsumed->type == TOKEN_VAR);
	char variable = variableConsumed ? lastConsumed->var : '\0';

	DEBUG_PRINT("[parser] consume(TOKEN_EQUALS)\n");
	bool equalsConsumed = consume(TOKEN_EQUALS);
	int errorCode = (equalsConsumed ? 0 : 1);
	g_parser->callback->symbol(errorCode, lastConsumed ? lastConsumed->symbol : '\0');

	number(false);
	bool numberConsumed = (lastConsumed && lastConsumed->type == TOKEN_NUMBER);
	float number = numberConsumed ? lastConsumed->number : 0.0f;

	errorCode = 0;
	if (!variableConsumed) errorCode += 1;
	if (!numberConsumed) errorCode += 2;
	g_parser->callback->letStmt(errorCode, variable, number);

	hasError = (errorCode != 0);
}

//------------------------------------------------------------------------------
// 5) <reset_stmt>	::= "RESET"
//------------------------------------------------------------------------------
void resetStmt()
{
	DEBUG_PRINT("[parser] resetStmt()\n");

	DEBUG_PRINT("[parser] consume(TOKEN_STMT_RESET)\n");
	bool resetConsumed = consume(TOKEN_STMT_RESET);
	int errorCode = (resetConsumed ? 0 : 1);
	g_parser->callback->resetStmt(errorCode);

	hasError = (errorCode != 0);
}

//------------------------------------------------------------------------------
// 6) <vars_stmt>	::= "VARS"
//------------------------------------------------------------------------------
void varsStmt()
{
	DEBUG_PRINT("[parser] varsStmt()\n");

	DEBUG_PRINT("[parser] consume(TOKEN_STMT_VARS)\n");
	bool varsConsumed = consume(TOKEN_STMT_VARS);
	int errorCode = (varsConsumed ? 0 : 1);
	g_parser->callback->varsStmt(errorCode);

	hasError = (errorCode != 0);
}

//------------------------------------------------------------------------------
// 7) <expr_stmt>	::= <expr>
//------------------------------------------------------------------------------
void exprStmt()
{
	DEBUG_PRINT("[parser] exprStmt()\n");

	g_parser->callback->exprStmtBegin(0);

	expr();

	// HACK: We know an <expr_stmt> reached its end when we find a line break.
	int errorCode = (g_parser->token->type == TOKEN_NEWLINE ? 0 : 1);
	g_parser->callback->exprStmtEnd(errorCode);

	hasError = (errorCode != 0);
}

//------------------------------------------------------------------------------
// 8) <expr>		::= <term> (("+" | "-") <term>)*
//------------------------------------------------------------------------------
void expr()
{
	DEBUG_PRINT("[parser] expr()\n");

	term();
	
	while (g_parser->token->type == TOKEN_ADD
		|| g_parser->token->type == TOKEN_SUB)
	{
		if (g_parser->token->type == TOKEN_ADD)
		{
			DEBUG_PRINT("[parser] consume(TOKEN_ADD)\n");
			bool addConsumed = consume(TOKEN_ADD);
			int errorCode = (addConsumed ? 0 : 1);
			g_parser->callback->symbol(errorCode, lastConsumed->symbol);
		}
		else if (g_parser->token->type == TOKEN_SUB)
		{
			DEBUG_PRINT("[parser] consume(TOKEN_SUB)\n");
			bool subConsumed = consume(TOKEN_SUB);
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

	while (g_parser->token->type == TOKEN_MUL
		|| g_parser->token->type == TOKEN_DIV)
	{
		if (g_parser->token->type == TOKEN_MUL)
		{
			DEBUG_PRINT("[parser] consume(TOKEN_MUL)\n");
			bool mulConsumed = consume(TOKEN_MUL);
			int errorCode = (mulConsumed ? 0 : 1);
			g_parser->callback->symbol(errorCode, lastConsumed->symbol);
		}
		else if (g_parser->token->type == TOKEN_DIV)
		{
			DEBUG_PRINT("[parser] consume(TOKEN_DIV)\n");
			bool divConsumed = consume(TOKEN_DIV);
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

	while (g_parser->token->type == TOKEN_POW)
	{
		DEBUG_PRINT("[parser] consume(TOKEN_POW)\n");
		bool powConsumed = consume(TOKEN_POW);
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

	if (g_parser->token->type == TOKEN_OPEN_PAREN)
	{
		DEBUG_PRINT("[parser] consume(TOKEN_OPEN_PAREN)\n");
		bool openParenConsumed = consume(TOKEN_OPEN_PAREN);
		int errorCode = (openParenConsumed ? 0 : 1);
		g_parser->callback->symbol(errorCode, lastConsumed->symbol);

		expr();

		DEBUG_PRINT("[parser] consume(TOKEN_CLOSE_PAREN)\n");
		bool closeParenConsumed = consume(TOKEN_CLOSE_PAREN);
		errorCode = (closeParenConsumed ? 0 : 1);
		g_parser->callback->symbol(errorCode, lastConsumed->symbol);
	}
	else if (g_parser->token->type == TOKEN_NEG)
	{
		negate();
		term();
	}
	else if (g_parser->token->type == TOKEN_VAR)
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

	DEBUG_PRINT("[parser] consume(TOKEN_VAR) = %c\n", g_parser->token->var);
	bool varConsumed = consume(TOKEN_VAR);

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

	bool isNegative = (g_parser->token->type == TOKEN_NEG);
	if (isNegative)
	{
		negate();
	}

	float number = g_parser->token->number;
	if (isNegative)
	{
		number = -number;
	}

	DEBUG_PRINT("[parser] consume(TOKEN_NUMBER) = %f\n", number);// g_parser->token->number);
	bool numberConsumed = consume(TOKEN_NUMBER);

	if (evaluate)
	{
		int errorCode = (numberConsumed ? 0 : 1);
		g_parser->callback->number(errorCode, lastConsumed->number);
	}
}

//------------------------------------------------------------------------------
// 14) <negate>		::= "~"
//------------------------------------------------------------------------------
void negate()
{
	DEBUG_PRINT("[parser] negate()\n");

	DEBUG_PRINT("[parser] consume(TOKEN_NEG)\n");
	bool negConsumed = consume(TOKEN_NEG);
	int errorCode = (negConsumed ? 0 : 1);
	g_parser->callback->symbol(errorCode, lastConsumed->symbol);
}

//------------------------------------------------------------------------------
// 15) <newline>	::= "\n" | "\r" | "\r\n"
//------------------------------------------------------------------------------
void newline()
{
	DEBUG_PRINT("[parser] newline()\n");

	DEBUG_PRINT("[parser] consume(TOKEN_NEWLINE)\n");
	bool newlineConsumed = consume(TOKEN_NEWLINE);
	int errorCode = (newlineConsumed ? 0 : 1);
	g_parser->callback->symbol(errorCode, lastConsumed->symbol);
}

//------------------------------------------------------------------------------
// TODO: *** DEBUG CODE - REMOVE ME FROM FINAL BUILD. ***
//------------------------------------------------------------------------------
void rpnStmt()
{
	DEBUG_PRINT("[parser] consume(TOKEN_STMT_RPN)\n");
	bool rpnConsumed = consume(TOKEN_STMT_RPN);
	int errorCode = (rpnConsumed ? 0 : 1);
	g_parser->callback->rpnStmt(errorCode);

	hasError = (errorCode != 0);
}
