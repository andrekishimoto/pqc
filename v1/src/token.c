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
// Includes
//------------------------------------------------------------------------------
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "token_symbols.h"
#include "utils.h"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktTokenPrint(const Token* token)
{
	switch (token->type)
	{
		case TOKEN_STRING:
			printf("STRING: %s\n", token->string);
			break;

		case TOKEN_VAR:
			printf("   VAR: %c\n", token->var);
			break;

		case TOKEN_NUMBER:
			printf("NUMBER: %f\n", token->number);
			break;

		case TOKEN_EQUALS:
		case TOKEN_ADD:
		case TOKEN_SUB:
		case TOKEN_MUL:
		case TOKEN_DIV:
		case TOKEN_POW:
		case TOKEN_OPEN_PAREN:
		case TOKEN_CLOSE_PAREN:
			printf("SYMBOL: %c\n", token->symbol);
			break;

		case TOKEN_STMT_LET:
		case TOKEN_STMT_RESET:
		case TOKEN_STMT_VARS:
			printf("  STMT: %s\n", token->string);
			break;
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
Token* ktTokenCreateString(const char* string)
{
	Token* token = malloc(sizeof(Token));
	if (token)
	{
		token->type = TOKEN_STRING;
		ktStringCopy(&token->string, string);
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
Token* ktTokenCreateVar(char var)
{
	Token* token = malloc(sizeof(Token));
	if (token)
	{
		token->type = TOKEN_VAR;
		token->var = toupper(var);
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
Token* ktTokenCreateNumber(float number)
{
	Token* token = malloc(sizeof(Token));
	if (token)
	{
		token->type = TOKEN_NUMBER;
		token->number = number;
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
Token* ktTokenCreateSymbol(TokenType type)
{
	Token* token = malloc(sizeof(Token));
	if (token)
	{
		token->type = type;
		switch (token->type)
		{
			case TOKEN_EQUALS:
				token->symbol = TOKEN_SYMBOL_EQUALS;
				break;
			case TOKEN_ADD:
				token->symbol = TOKEN_SYMBOL_ADD;
				break;
			case TOKEN_SUB:
				token->symbol = TOKEN_SYMBOL_SUB;
				break;
			case TOKEN_MUL:
				token->symbol = TOKEN_SYMBOL_MUL;
				break;
			case TOKEN_DIV:
				token->symbol = TOKEN_SYMBOL_DIV;
				break;
			case TOKEN_POW:
				token->symbol = TOKEN_SYMBOL_POW;
				break;
			case TOKEN_OPEN_PAREN:
				token->symbol = TOKEN_SYMBOL_OPEN_PAREN;
				break;
			case TOKEN_CLOSE_PAREN:
				token->symbol = TOKEN_SYMBOL_CLOSE_PAREN;
				break;
			default:
				token->symbol = '\0';
				break;
		}
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
Token* ktTokenCreateStmtLet()
{
	Token* token = malloc(sizeof(Token));
	if (token)
	{
		token->type = TOKEN_STMT_LET;
		ktStringCopy(&token->string, TOKEN_STMT_LET_VALUE);
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
Token* ktTokenCreateStmtReset()
{
	Token* token = malloc(sizeof(Token));
	if (token)
	{
		token->type = TOKEN_STMT_RESET;
		ktStringCopy(&token->string, TOKEN_STMT_RESET_VALUE);
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
Token* ktTokenCreateStmtVars()
{
	Token* token = malloc(sizeof(Token));
	if (token)
	{
		token->type = TOKEN_STMT_VARS;
		ktStringCopy(&token->string, TOKEN_STMT_VARS_VALUE);
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktTokenDestroy(Token* token)
{
	if (token)
	{
		bool mustDestroyString = token->type == TOKEN_STRING
			|| token->type == TOKEN_STMT_LET
			|| token->type == TOKEN_STMT_RESET
			|| token->type == TOKEN_STMT_VARS;
		if (mustDestroyString)
		{
			ktStringDestroy(&token->string);
		}

		SAFE_DELETE(token);
	}
}
