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
static void expr();
static void term();
static void factor();
static void base();
static void var();
static void number();
static void negate();
static void newline();

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Parser* ktParserCreate()
{
	Parser* parser = malloc(sizeof(Parser));
	if (parser)
	{
		parser->tokenList = ktTokenListCreate();
		parser->token = NULL;
		parser->index = -1;
		parser->memory = ktMemoryCreate();
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
		ktMemoryDestroy(parser->memory);
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
	ktTokenizerRun(contents, parser->tokenList);
	start();

	if (g_parser->token->type != TOKEN_EOF)
	{
		// TODO: If we landed here, then it is an error!
		// Expected EOF.
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
		printf("[parser] ERROR: There are no more tokens to parse (unexpected TOKEN_EOF).\n");
		
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
		printf("[parser] ERROR: Incorrect token. Expected: %s, got: %s.\n", TOKEN_TYPE_STR[expected], TOKEN_TYPE_STR[g_parser->token->type]);

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
		|| g_parser->token->type == TOKEN_OPEN_PAREN
		|| g_parser->token->type == TOKEN_VAR
		|| g_parser->token->type == TOKEN_NEG
		|| g_parser->token->type == TOKEN_NEWLINE)
	{
		if (g_parser->token->type == TOKEN_STMT_LET
			|| g_parser->token->type == TOKEN_STMT_RESET
			|| g_parser->token->type == TOKEN_STMT_VARS
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
	else
	{
		exprStmt();
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

	var();
	char variable = (lastConsumed && lastConsumed->type == TOKEN_VAR) ? lastConsumed->var : '\0';

	DEBUG_PRINT("[parser] consume(TOKEN_EQUALS)\n");
	consume(TOKEN_EQUALS);

	number();
	float number = (lastConsumed && lastConsumed->type == TOKEN_NUMBER) ? lastConsumed->number : 0.0f;

	if (variable != '\0')
	{
		size_t index = (size_t)variable - (size_t)'A';

		if (lastConsumed && lastConsumed->type == TOKEN_NUMBER)
		{
			ktMemorySet(g_parser->memory, index, number);
		}

		printf("%c = %f\n", (char)(index + 'A'), g_parser->memory->vars[index]);
	}
}

//------------------------------------------------------------------------------
// 5) <reset_stmt>	::= "RESET"
//------------------------------------------------------------------------------
void resetStmt()
{
	DEBUG_PRINT("[parser] resetStmt()\n");

	DEBUG_PRINT("[parser] consume(TOKEN_STMT_RESET)\n");
	if (consume(TOKEN_STMT_RESET))
	{
		printf("Resetting all variables... ");
		ktMemoryReset(g_parser->memory);
		printf("Done.\n");
	}
}

//------------------------------------------------------------------------------
// 6) <vars_stmt>	::= "VARS"
//------------------------------------------------------------------------------
void varsStmt()
{
	DEBUG_PRINT("[parser] varsStmt()\n");

	DEBUG_PRINT("[parser] consume(TOKEN_STMT_VARS)\n");
	if (consume(TOKEN_STMT_VARS))
	{
		printf("Please note: The list below omits variables that are not set yet.\n");
		for (size_t i = 0; i < VAR_COUNT; ++i)
		{
			if (g_parser->memory->hasValue[i])
			{
				printf("%c = %f\n", (char)(i + 'A'), g_parser->memory->vars[i]);
			}
		}
	}
}

//------------------------------------------------------------------------------
// 7) <expr_stmt>	::= <expr>
//------------------------------------------------------------------------------
void exprStmt()
{
	DEBUG_PRINT("[parser] exprStmt()\n");

	expr();
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
		}
		else if (g_parser->token->type == TOKEN_SUB)
		{
			DEBUG_PRINT("[parser] consume(TOKEN_SUB)\n");
		}
		advance();

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
		}
		else if (g_parser->token->type == TOKEN_DIV)
		{
			DEBUG_PRINT("[parser] consume(TOKEN_DIV)\n");
		}
		advance();

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
		advance();

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
		advance();// consume(TOKEN_OPEN_PAREN);

		expr();

		DEBUG_PRINT("[parser] consume(TOKEN_CLOSE_PAREN)\n");
		consume(TOKEN_CLOSE_PAREN);
	}
	else if (g_parser->token->type == TOKEN_NEG)
	{
		negate();
		term();
	}
	else if (g_parser->token->type == TOKEN_VAR)
	{
		var();
	}
}

//------------------------------------------------------------------------------
// 12) <var>		::= [A-Z]
//------------------------------------------------------------------------------
void var()
{
	DEBUG_PRINT("[parser] var()\n");

	DEBUG_PRINT("[parser] consume(TOKEN_VAR) = %c\n", g_parser->token->var);
	consume(TOKEN_VAR);
}

//------------------------------------------------------------------------------
// 13) <number>		::= <negate>? [0-9]+ ("." [0-9]+)?
//------------------------------------------------------------------------------
void number()
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
	consume(TOKEN_NUMBER);
}

//------------------------------------------------------------------------------
// 14) <negate>		::= "~"
//------------------------------------------------------------------------------
void negate()
{
	DEBUG_PRINT("[parser] negate()\n");

	DEBUG_PRINT("[parser] consume(TOKEN_NEG)\n");
	consume(TOKEN_NEG);
}

//------------------------------------------------------------------------------
// 15) <newline>	::= "\n" | "\r" | "\r\n"
//------------------------------------------------------------------------------
void newline()
{
	DEBUG_PRINT("[parser] newline()\n");

	DEBUG_PRINT("[parser] consume(TOKEN_NEWLINE)\n");
	consume(TOKEN_NEWLINE);
}
