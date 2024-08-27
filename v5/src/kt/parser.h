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
#include "debug.h"

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
typedef struct ktParser ktParser;
typedef struct ktParserCallback ktParserCallback;

struct ktParser
{
	ktTokenList* tokenList;
	ktToken* token;
	int index;

	ktParserCallback* callback;
};

struct ktParserCallback
{
	void (*letStmt)(int errorCode, char variable, float value);
	void (*resetStmt)(int errorCode);
	void (*varsStmt)(int errorCode);
	void (*exprStmtBegin)(int errorCode);
	void (*exprStmtEnd)(int errorCode);
	void (*var)(int errorCode, char variable);
	void (*number)(int errorCode, float number);
	void (*symbol)(int errorCode, char symbol);
	void (*error)(int errorCode, const char* message);

#if _DEBUG_RPN
	void (*rpnStmt)(int errorCode);
#endif // #if _DEBUG_RPN
};

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
ktParser* ktParserCreate(ktParserCallback* callback);
void ktParserDestroy(ktParser* parser);
void ktParserRun(ktParser* parser, const char* contents);

#endif // __KISHITECH_PARSER_H__
