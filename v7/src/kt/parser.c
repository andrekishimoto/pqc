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
// 3) <stmt_list>	::= <let_stmt> | <reset_stmt> | <vars_stmt> | <clear_stmt> | <exit_stmt> | <expr_stmt>
// 4) <let_stmt>	::= "LET" <var> "=" <number>
// 5) <reset_stmt>	::= "RESET"
// 6) <vars_stmt>	::= "VARS"
// 7) <clear_stmt>	::= "CLEAR"
// 8) <exit_stmt>	::= "EXIT"
// 9) <expr_stmt>	::= <expr>
// 10) <expr>		::= <term> (("+" | "-") <term>)*
// 11) <term>		::= <factor> (("*" | "/") <factor>)*
// 12) <factor>		::= <base> ("^" <factor>)*
// 13) <base>		::= "(" <expr> ")" | (<negate> <term>) | <var>
// 14) <var>		::= [A-Z]
// 15) <number>		::= <negate>? [0-9]+ ("." [0-9]+)?
// 16) <negate>		::= "~"
// 17) <newline>	::= "\n" | "\r" | "\r\n"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stdio.h>
#include "parser.h"
#include "tokenizer.h"
#include "token_list.h"
#include "token.h"
#include "consts.h"
#include "error_type.h"
#include "utils.h"

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
typedef struct ktParser ktParser;

struct ktParser
{
	ktTokenList* tokenList;
	ktToken* token;
	int index;

	ktParserCallback* callback;
};

//------------------------------------------------------------------------------
// Globals (argh!)
//------------------------------------------------------------------------------
static ktParser* g_parser = NULL;
static ktToken* lastConsumed = NULL;

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
static void reset(void);
static void start(void);
static bool advance(void);
#if 0
static ktToken* peek(size_t ahead);
#endif
static bool consume(ktTokenType expected);

static void program(void);
static void stmt(void);
static void letStmt(void);
static void resetStmt(void);
static void varsStmt(void);
static void clearStmt(void);
static void exitStmt(void);
static void exprStmt(void);
static void expr(void);
static void term(void);
static void factor(void);
static void base(void);
static void var(bool evaluate);
static void number(bool evaluate);
static void negate(bool evaluate);
static void newline(void);
static void callbackSymbol(bool consumed);

#if _DEBUG_RPN
static void rpnStmt(void);
#endif // #if _DEBUG_RPN

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktParserCreate(ktParserCallback* callback)
{
	g_parser = malloc(sizeof(ktParser));
	if (g_parser)
	{
		g_parser->tokenList = ktTokenListCreate();
		g_parser->token = NULL;
		g_parser->index = -1;
		g_parser->callback = callback;
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktParserDestroy()
{
	if (g_parser)
	{
		ktTokenListDestroy(g_parser->tokenList);
		SAFE_DELETE(g_parser);
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktParserRun(const char* contents)
{
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
			char buffer[KT_ERROR_MESSAGE_MAX_LENGTH] = { 0 };
			snprintf(buffer, KT_ERROR_MESSAGE_MAX_LENGTH, ktErrorDescription(KT_ERROR_PARSER_UNKNOWN_COMMAND), g_parser->token->string);
			g_parser->callback->error(KT_ERROR_PARSER_UNKNOWN_COMMAND, buffer);
		}
		else if (g_parser->token->type == KT_TOKEN_NUMBER || g_parser->token->type == KT_TOKEN_EQUALS)
		{
			g_parser->callback->error(KT_ERROR_PARSER_DID_YOU_MEAN_LET, ktErrorDescription(KT_ERROR_PARSER_DID_YOU_MEAN_LET));
		}
		else if (g_parser->token->type == KT_TOKEN_ERROR)
		{
			g_parser->callback->error(KT_ERROR_PARSER_TOKENIZER_ERROR, g_parser->token->string);
		}
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void reset(void)
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
void start(void)
{
	advance();
	program();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool advance(void)
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

#if 0
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
#endif

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
		char buffer[KT_ERROR_MESSAGE_MAX_LENGTH] = { 0 };
		snprintf(buffer, KT_ERROR_MESSAGE_MAX_LENGTH, ktErrorDescription(KT_ERROR_PARSER_CONSUME_EXPECTED_GOT), KT_TOKEN_TYPE_STR[expected], KT_TOKEN_TYPE_STR[g_parser->token->type]);
		g_parser->callback->error(KT_ERROR_PARSER_CONSUME_EXPECTED_GOT, buffer);

		// HACK: Since there is an error, let's skip right to the next KT_TOKEN_NEWLINE in the token list.
		size_t steps = (g_parser->tokenList->count - 1) - g_parser->index;
		while (steps && g_parser->tokenList->cursor->token->type != KT_TOKEN_NEWLINE)
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
void program(void)
{
	DEBUG_PRINT("[parser] program()\n");

	while (g_parser->token->type == KT_TOKEN_STMT_LET
		|| g_parser->token->type == KT_TOKEN_STMT_RESET
		|| g_parser->token->type == KT_TOKEN_STMT_VARS
		|| g_parser->token->type == KT_TOKEN_STMT_CLEAR
		|| g_parser->token->type == KT_TOKEN_STMT_EXIT
		|| g_parser->token->type == KT_TOKEN_OPEN_PAREN
		|| g_parser->token->type == KT_TOKEN_VAR
		|| g_parser->token->type == KT_TOKEN_NEG
		|| g_parser->token->type == KT_TOKEN_NEWLINE

		// The tokens below were added so the interpreter outputs
		// the same exprStmt error if a string begins with an operator
		// or a symbol that is related to an exprStmt.
		|| g_parser->token->type == KT_TOKEN_ADD
		|| g_parser->token->type == KT_TOKEN_SUB
		|| g_parser->token->type == KT_TOKEN_MUL
		|| g_parser->token->type == KT_TOKEN_DIV
		|| g_parser->token->type == KT_TOKEN_POW
		|| g_parser->token->type == KT_TOKEN_CLOSE_PAREN

#if _DEBUG_RPN
		|| g_parser->token->type == KT_TOKEN_STMT_RPN
#endif // #if _DEBUG_RPN
	)
	{
		switch (g_parser->token->type)
		{
		case KT_TOKEN_STMT_LET:
		case KT_TOKEN_STMT_RESET:
		case KT_TOKEN_STMT_VARS:
		case KT_TOKEN_STMT_CLEAR:
		case KT_TOKEN_STMT_EXIT:
		case KT_TOKEN_OPEN_PAREN:
		case KT_TOKEN_VAR:
		case KT_TOKEN_NEG:
			// The tokens below were added so the interpreter outputs
			// the same exprStmt error if a string begins with an operator
			// or a symbol that is related to an exprStmt.
		case KT_TOKEN_ADD:
		case KT_TOKEN_SUB:
		case KT_TOKEN_MUL:
		case KT_TOKEN_DIV:
		case KT_TOKEN_POW:
		case KT_TOKEN_CLOSE_PAREN:

#if _DEBUG_RPN
		case KT_TOKEN_STMT_RPN:
#endif // #if _DEBUG_RPN
			stmt();
			break;

		case KT_TOKEN_NEWLINE:
		default:
			advance();
			break;
		}
	}
}

//------------------------------------------------------------------------------
// 2) <stmt>		::= <stmt_list> <newline>
// 3) <stmt_list>	::= <let_stmt> | <reset_stmt> | <vars_stmt> | <clear_stmt> | <exit_stmt> | <expr_stmt>
//------------------------------------------------------------------------------
void stmt(void)
{
	DEBUG_PRINT("[parser] stmt()\n");

	switch (g_parser->token->type)
	{
	case KT_TOKEN_STMT_LET:
		letStmt();
		break;

	case KT_TOKEN_STMT_RESET:
		resetStmt();
		break;

	case KT_TOKEN_STMT_VARS:
		varsStmt();
		break;

	case KT_TOKEN_STMT_CLEAR:
		clearStmt();
		break;

	case KT_TOKEN_STMT_EXIT:
		exitStmt();
		break;

#if _DEBUG_RPN
	case KT_TOKEN_STMT_RPN:
		rpnStmt();
		break;
#endif // #if _DEBUG_RPN

	default:
		exprStmt();
		break;
	}
}

//------------------------------------------------------------------------------
// 4) <let_stmt>	::= "LET" <var> "=" <number>
//------------------------------------------------------------------------------
void letStmt(void)
{
	DEBUG_PRINT("[parser] letStmt()\n");

	DEBUG_PRINT("[parser] consume(KT_TOKEN_STMT_LET)\n");
	consume(KT_TOKEN_STMT_LET);

	var(false);
	bool variableConsumed = (lastConsumed && lastConsumed->type == KT_TOKEN_VAR);
	char variable = variableConsumed ? lastConsumed->var : '\0';

	DEBUG_PRINT("[parser] consume(KT_TOKEN_EQUALS)\n");
	bool equalsConsumed = consume(KT_TOKEN_EQUALS);
	callbackSymbol(equalsConsumed);

	number(false);
	bool numberConsumed = (lastConsumed && lastConsumed->type == KT_TOKEN_NUMBER);
	double number = numberConsumed ? lastConsumed->number : 0.0;

	newline();
	bool newlineConsumed = (lastConsumed && lastConsumed->type == KT_TOKEN_NEWLINE);
	
	int errorCode = 0;
	if (!variableConsumed) errorCode |= KT_LET_STMT_VAR_FLAG;
	if (!numberConsumed) errorCode |= KT_LET_STMT_VALUE_FLAG;
	if (!newlineConsumed) errorCode |= KT_LET_STMT_PARAMS_FLAG;
	g_parser->callback->letStmt(errorCode, variable, number);
}

//------------------------------------------------------------------------------
// 5) <reset_stmt>	::= "RESET"
//------------------------------------------------------------------------------
void resetStmt(void)
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
void varsStmt(void)
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
// 7) <clear_stmt>	::= "CLEAR"
//------------------------------------------------------------------------------
void clearStmt(void)
{
	DEBUG_PRINT("[parser] clearStmt()\n");

	DEBUG_PRINT("[parser] consume(KT_TOKEN_STMT_CLEAR)\n");
	bool clearConsumed = consume(KT_TOKEN_STMT_CLEAR);

	newline();
	//bool newlineConsumed = (lastConsumed && lastConsumed->type == KT_TOKEN_NEWLINE);
	//int errorCode = (clearConsumed && newlineConsumed ? 0 : 1);

	if (clearConsumed)
	{
		g_parser->callback->clearStmt();
	}
}

//------------------------------------------------------------------------------
// 8) <exit_stmt>	::= "EXIT"
//------------------------------------------------------------------------------
void exitStmt(void)
{
	DEBUG_PRINT("[parser] exitStmt()\n");

	DEBUG_PRINT("[parser] consume(KT_TOKEN_STMT_EXIT)\n");
	bool exitConsumed = consume(KT_TOKEN_STMT_EXIT);

	newline();
	//bool newlineConsumed = (lastConsumed && lastConsumed->type == KT_TOKEN_NEWLINE);
	//int errorCode = (exitConsumed && newlineConsumed ? 0 : 1);

	if (exitConsumed)
	{
		g_parser->callback->exitStmt();
	}
}

//------------------------------------------------------------------------------
// 9) <expr_stmt>	::= <expr>
//------------------------------------------------------------------------------
void exprStmt(void)
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
// 10) <expr>		::= <term> (("+" | "-") <term>)*
//------------------------------------------------------------------------------
void expr(void)
{
	DEBUG_PRINT("[parser] expr()\n");

	term();
	
	while (g_parser->token->type == KT_TOKEN_ADD || g_parser->token->type == KT_TOKEN_SUB)
	{
		if (g_parser->token->type == KT_TOKEN_ADD)
		{
			DEBUG_PRINT("[parser] consume(KT_TOKEN_ADD)\n");
			bool addConsumed = consume(KT_TOKEN_ADD);
			callbackSymbol(addConsumed);

		}
		else if (g_parser->token->type == KT_TOKEN_SUB)
		{
			DEBUG_PRINT("[parser] consume(KT_TOKEN_SUB)\n");
			bool subConsumed = consume(KT_TOKEN_SUB);
			callbackSymbol(subConsumed);
		}

		term();
	}
}

//------------------------------------------------------------------------------
// 11) <term>		::= <factor> (("*" | "/") <factor>)*
//------------------------------------------------------------------------------
void term(void)
{
	DEBUG_PRINT("[parser] term()\n");

	factor();

	while (g_parser->token->type == KT_TOKEN_MUL || g_parser->token->type == KT_TOKEN_DIV)
	{
		if (g_parser->token->type == KT_TOKEN_MUL)
		{
			DEBUG_PRINT("[parser] consume(KT_TOKEN_MUL)\n");
			bool mulConsumed = consume(KT_TOKEN_MUL);
			callbackSymbol(mulConsumed);
		}
		else if (g_parser->token->type == KT_TOKEN_DIV)
		{
			DEBUG_PRINT("[parser] consume(KT_TOKEN_DIV)\n");
			bool divConsumed = consume(KT_TOKEN_DIV);
			callbackSymbol(divConsumed);
		}

		factor();
	}
}

//------------------------------------------------------------------------------
// 12) <factor>		::= <base> ("^" <factor>)*
//------------------------------------------------------------------------------
void factor(void)
{
	DEBUG_PRINT("[parser] factor()\n");

	base();

	while (g_parser->token->type == KT_TOKEN_POW)
	{
		DEBUG_PRINT("[parser] consume(KT_TOKEN_POW)\n");
		bool powConsumed = consume(KT_TOKEN_POW);
		callbackSymbol(powConsumed);

		factor();
	}
}

//------------------------------------------------------------------------------
// 13) <base>		::= "(" <expr> ")" | (<negate> <term>) | <var>
//------------------------------------------------------------------------------
void base(void)
{
	DEBUG_PRINT("[parser] base()\n");

	if (g_parser->token->type == KT_TOKEN_OPEN_PAREN)
	{
		DEBUG_PRINT("[parser] consume(KT_TOKEN_OPEN_PAREN)\n");
		bool openParenConsumed = consume(KT_TOKEN_OPEN_PAREN);
		callbackSymbol(openParenConsumed);

		expr();

		DEBUG_PRINT("[parser] consume(KT_TOKEN_CLOSE_PAREN)\n");
		bool closeParenConsumed = consume(KT_TOKEN_CLOSE_PAREN);
		callbackSymbol(closeParenConsumed);
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
// 14) <var>		::= [A-Z]
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
// 15) <number>		::= <negate>? [0-9]+ ("." [0-9]+)?
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
// 16) <negate>		::= "~"
//------------------------------------------------------------------------------
void negate(bool evaluate)
{
	DEBUG_PRINT("[parser] negate()\n");

	DEBUG_PRINT("[parser] consume(KT_TOKEN_NEG)\n");
	bool negConsumed = consume(KT_TOKEN_NEG);
	
	if (evaluate)
	{
		callbackSymbol(negConsumed);
	}
}

//------------------------------------------------------------------------------
// 17) <newline>	::= "\n" | "\r" | "\r\n"
//------------------------------------------------------------------------------
void newline(void)
{
	DEBUG_PRINT("[parser] newline()\n");

	DEBUG_PRINT("[parser] consume(KT_TOKEN_NEWLINE)\n");
	bool newlineConsumed = consume(KT_TOKEN_NEWLINE);
	callbackSymbol(newlineConsumed);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void callbackSymbol(bool consumed)
{
	int errorCode = consumed ? 0 : 1;

	g_parser->callback->symbol(errorCode,
		errorCode == 0
		? (lastConsumed ? lastConsumed->symbol : '\0')
		: '\0');
}

#if _DEBUG_RPN
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void rpnStmt(void)
{
	DEBUG_PRINT("[parser] consume(KT_TOKEN_STMT_RPN)\n");
	bool rpnConsumed = consume(KT_TOKEN_STMT_RPN);
	int errorCode = (rpnConsumed ? 0 : 1);
	g_parser->callback->rpnStmt(errorCode);
}
#endif // #if _DEBUG_RPN
