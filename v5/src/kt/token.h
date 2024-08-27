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

#ifndef __KISHITECH_TOKEN_H__
#define __KISHITECH_TOKEN_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "token_type.h"
#include "debug.h"

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
typedef struct ktToken ktToken;

struct ktToken
{
	ktTokenType type;
	
	union
	{
		char* string;
		char var;
		char symbol;
		float number;
	};
};

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
ktToken* ktTokenCreateWord(const char* string);
ktToken* ktTokenCreateVar(char var);
ktToken* ktTokenCreateNumber(float number);
ktToken* ktTokenCreateSymbol(ktTokenType type);
ktToken* ktTokenCreateStmtLet();
ktToken* ktTokenCreateStmtReset();
ktToken* ktTokenCreateStmtVars();
ktToken* ktTokenCreateNotSupported(char ch);
ktToken* ktTokenCreateError(const char* string);
void ktTokenDestroy(ktToken* token);
void ktTokenPrint(const ktToken* token);

#if _DEBUG_RPN
Token* ktTokenCreateStmtRpn();
#endif // #if _DEBUG_RPN

#endif // __KISHITECH_TOKEN_H__
