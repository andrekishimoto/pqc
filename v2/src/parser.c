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
#include "parser.h"
#include "tokenizer.h"
#include "utils.h"

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
static void reset(Parser* parser);
static void start(Parser* parser);
static void advance(Parser* parser);
static Token* peek(Parser* parser, size_t ahead);
static void consume(Parser* parser, TokenType expected);

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
		parser->tokens = ktTokenListCreate();
		parser->current = NULL;
		parser->index = -1;
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
		ktTokenListDestroy(parser->tokens);
		SAFE_DELETE(parser);
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktParserRun(Parser* parser, char* contents)
{
	reset(parser);
	ktTokenizerRun(ktStringToUpper(contents), parser->tokens);
	start(parser);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void reset(Parser* parser)
{
	if (!parser)
		return;

	ktTokenListClear(parser->tokens);
	parser->current = NULL;
	parser->index = -1;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void start(Parser* parser)
{
	advance(parser);
	program();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void advance(Parser* parser)
{
	++parser->index;
	if (parser->index >= (int)parser->tokens->count)
	{
		// TODO: If we landed here, then it is an error!
		// There are no more tokens to parse (unexpected EOF).
	}
	else
	{
		if (parser->index > 0)
		{
			ktTokenListCursorAdvance(parser->tokens);
		}
		parser->current = parser->tokens->cursor->token;
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Token* peek(Parser* parser, size_t ahead)
{
	if (!parser)
		return NULL;

	if (parser->index + ahead < parser->tokens->count)
	{
		TokenNode* originalCursor = parser->tokens->cursor;
		for (size_t i = 0; i < ahead; ++i)
		{
			ktTokenListCursorAdvance(parser->tokens);
		}
		Token* token = parser->tokens->cursor->token;
		parser->tokens->cursor = originalCursor;

		return token;
	}

	return NULL;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void consume(Parser* parser, TokenType expected)
{
	if (parser->current->type == expected)
	{
		advance(parser);
	}
	else
	{
		// TODO: If we landed here, then it is an error!
		// Incorrect token - Expected: expected. Got: parser->current->type.
	}
}

//------------------------------------------------------------------------------
// 1) <program>		::= (<stmt> | <newline>)* 
//------------------------------------------------------------------------------
void program()
{

}

//------------------------------------------------------------------------------
// 2) <stmt>		::= <stmt_list> <newline>
// 3) <stmt_list>	::= <let_stmt> | <reset_stmt> | <vars_stmt> | <expr_stmt>
//------------------------------------------------------------------------------
void stmt()
{

}

//------------------------------------------------------------------------------
// 4) <let_stmt>	::= "LET" <var> "=" <number>
//------------------------------------------------------------------------------
void letStmt()
{

}

//------------------------------------------------------------------------------
// 5) <reset_stmt>	::= "RESET"
//------------------------------------------------------------------------------
void resetStmt()
{

}

//------------------------------------------------------------------------------
// 6) <vars_stmt>	::= "VARS"
//------------------------------------------------------------------------------
void varsStmt()
{

}

//------------------------------------------------------------------------------
// 7) <expr_stmt>	::= <expr>
//------------------------------------------------------------------------------
void exprStmt()
{

}

//------------------------------------------------------------------------------
// 8) <expr>		::= <term> (("+" | "-") <term>)*
//------------------------------------------------------------------------------
void expr()
{

}

//------------------------------------------------------------------------------
// 9) <term>		::= <factor> (("*" | "/") <factor>)*
//------------------------------------------------------------------------------
void term()
{

}

//------------------------------------------------------------------------------
// 10) <factor>		::= <base> ("^" <factor>)*
//------------------------------------------------------------------------------
void factor()
{

}

//------------------------------------------------------------------------------
// 11) <base>		::= "(" <expr> ")" | (<negate> <term>) | <var>
//------------------------------------------------------------------------------
void base()
{

}

//------------------------------------------------------------------------------
// 12) <var>		::= [A-Z]
//------------------------------------------------------------------------------
void var()
{

}

//------------------------------------------------------------------------------
// 13) <number>		::= <negate>? [0-9]+ ("." [0-9]+)?
//------------------------------------------------------------------------------
void number()
{

}

//------------------------------------------------------------------------------
// 14) <negate>		::= "~"
//------------------------------------------------------------------------------
void negate()
{

}

//------------------------------------------------------------------------------
// 15) <newline>	::= "\n" | "\r" | "\r\n"
//------------------------------------------------------------------------------
void newline()
{

}
