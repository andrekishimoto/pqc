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
ktToken* ktTokenCreateWord(const char* string)
{
	ktToken* token = malloc(sizeof(ktToken));
	if (token)
	{
		token->type = KT_TOKEN_WORD;
		ktStringCopy(&token->string, string);
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ktToken* ktTokenCreateVar(char var)
{
	ktToken* token = malloc(sizeof(ktToken));
	if (token)
	{
		token->type = KT_TOKEN_VAR;
		token->var = toupper(var);
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ktToken* ktTokenCreateNumber(float number)
{
	ktToken* token = malloc(sizeof(ktToken));
	if (token)
	{
		token->type = KT_TOKEN_NUMBER;
		token->number = number;
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ktToken* ktTokenCreateSymbol(ktTokenType type)
{
	ktToken* token = malloc(sizeof(ktToken));
	if (token)
	{
		token->type = type;
		switch (token->type)
		{
			case KT_TOKEN_WHITESPACE:
				token->symbol = KT_TOKEN_WHITESPACE_SYMBOL;
				break;
			case KT_TOKEN_NEWLINE:
				token->symbol = KT_TOKEN_NEWLINE_SYMBOL;
				break;
			case KT_TOKEN_EOF:
				token->symbol = KT_TOKEN_EOF_SYMBOL;
				break;
			case KT_TOKEN_EQUALS:
				token->symbol = KT_TOKEN_EQUALS_SYMBOL;
				break;
			case KT_TOKEN_ADD:
				token->symbol = KT_TOKEN_ADD_SYMBOL;
				break;
			case KT_TOKEN_SUB:
				token->symbol = KT_TOKEN_SUB_SYMBOL;
				break;
			case KT_TOKEN_MUL:
				token->symbol = KT_TOKEN_MUL_SYMBOL;
				break;
			case KT_TOKEN_DIV:
				token->symbol = KT_TOKEN_DIV_SYMBOL;
				break;
			case KT_TOKEN_POW:
				token->symbol = KT_TOKEN_POW_SYMBOL;
				break;
			case KT_TOKEN_NEG:
				token->symbol = KT_TOKEN_NEG_SYMBOL;
				break;
			case KT_TOKEN_OPEN_PAREN:
				token->symbol = KT_TOKEN_OPEN_PAREN_SYMBOL;
				break;
			case KT_TOKEN_CLOSE_PAREN:
				token->symbol = KT_TOKEN_CLOSE_PAREN_SYMBOL;
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
ktToken* ktTokenCreateStmtLet()
{
	ktToken* token = malloc(sizeof(ktToken));
	if (token)
	{
		token->type = KT_TOKEN_STMT_LET;
		ktStringCopy(&token->string, KT_TOKEN_STMT_LET_VALUE);
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ktToken* ktTokenCreateStmtReset()
{
	ktToken* token = malloc(sizeof(ktToken));
	if (token)
	{
		token->type = KT_TOKEN_STMT_RESET;
		ktStringCopy(&token->string, KT_TOKEN_STMT_RESET_VALUE);
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ktToken* ktTokenCreateStmtVars()
{
	ktToken* token = malloc(sizeof(ktToken));
	if (token)
	{
		token->type = KT_TOKEN_STMT_VARS;
		ktStringCopy(&token->string, KT_TOKEN_STMT_VARS_VALUE);
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ktToken* ktTokenCreateNotSupported(char ch)
{
	ktToken* token = malloc(sizeof(ktToken));
	if (token)
	{
		token->type = KT_TOKEN_NOT_SUPPORTED;
		token->symbol = ch;
	}

	return token;

}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ktToken* ktTokenCreateError(const char* string)
{
	ktToken* token = malloc(sizeof(ktToken));
	if (token)
	{
		token->type = KT_TOKEN_ERROR;
		ktStringCopy(&token->string, string);
	}

	return token;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktTokenDestroy(ktToken* token)
{
	if (token)
	{
		bool mustDestroyString = token->type == KT_TOKEN_WORD
			|| token->type == KT_TOKEN_STMT_LET
			|| token->type == KT_TOKEN_STMT_RESET
			|| token->type == KT_TOKEN_STMT_VARS
			|| token->type == KT_TOKEN_ERROR;
		if (mustDestroyString)
		{
			ktStringDestroy(&token->string);
		}

		SAFE_DELETE(token);
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktTokenPrint(const ktToken* token)
{
	switch (token->type)
	{
	case KT_TOKEN_WORD:
		printf("STRING: %s\n", token->string);
		break;

	case KT_TOKEN_VAR:
		printf("   VAR: %c\n", token->var);
		break;

	case KT_TOKEN_NUMBER:
		printf("NUMBER: %f\n", token->number);
		break;

	case KT_TOKEN_WHITESPACE:
		printf("ESCAPE: WHITESPACE\n");
		break;

	case KT_TOKEN_NEWLINE:
		printf("ESCAPE: NEWLINE\n");
		break;

	case KT_TOKEN_EOF:
		printf("ESCAPE: EOF\n");
		break;

	case KT_TOKEN_EQUALS:
	case KT_TOKEN_ADD:
	case KT_TOKEN_SUB:
	case KT_TOKEN_MUL:
	case KT_TOKEN_DIV:
	case KT_TOKEN_POW:
	case KT_TOKEN_OPEN_PAREN:
	case KT_TOKEN_CLOSE_PAREN:
		printf("SYMBOL: %c\n", token->symbol);
		break;

	case KT_TOKEN_NEG:
		printf("SYMBOL: %c (KT_TOKEN_NEG)\n", token->symbol);
		break;

	case KT_TOKEN_STMT_LET:
	case KT_TOKEN_STMT_RESET:
	case KT_TOKEN_STMT_VARS:
		printf("  STMT: %s\n", token->string);
		break;

	case KT_TOKEN_NOT_SUPPORTED:
		printf("SYMBOL: %c (KT_TOKEN_NOT_SUPPORTED)\n", token->symbol);
		break;

	case KT_TOKEN_ERROR:
		printf(" ERROR: %s\n", token->string);
		break;

#if _DEBUG_RPN
	case KT_TOKEN_STMT_RPN:
		printf("  STMT: %s (DEBUG)\n", token->string);
		break;
#endif
	}
}

#if _DEBUG_RPN
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Token* ktTokenCreateStmtRpn()
{
	Token* token = malloc(sizeof(Token));
	if (token)
	{
		token->type = KT_TOKEN_STMT_RPN;
		ktStringCopy(&token->string, KT_TOKEN_STMT_RPN_VALUE);
	}

	return token;
}
#endif // #if _DEBUG_RPN
