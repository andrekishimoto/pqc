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

#ifndef __KISHITECH_PARSER_H__
#define __KISHITECH_PARSER_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "token_list.h"

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
typedef struct Parser Parser;
typedef struct ParserCallback ParserCallback;

struct Parser
{
	TokenList* tokenList;
	Token* token;
	int index;

	ParserCallback* callback;
};

struct ParserCallback
{
	void (*letStmt)(int errorCode, char variable, float value);
	void (*resetStmt)(int errorCode);
	void (*varsStmt)(int errorCode);
	void (*exprStmtBegin)(int errorCode);
	void (*exprStmtEnd)(int errorCode);
	void (*rpnStmt)(int errorCode); // TODO: *** DEBUG CODE - REMOVE ME FROM FINAL BUILD. ***
	void (*var)(int errorCode, char variable);
	void (*number)(int errorCode, float number);
	void (*symbol)(int errorCode, char symbol);
	void (*error)(int errorCode, const char* message);
};

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
Parser* ktParserCreate(ParserCallback* callback);
void ktParserDestroy(Parser* parser);
void ktParserRun(Parser* parser, const char* contents);

#endif // __KISHITECH_PARSER_H__
