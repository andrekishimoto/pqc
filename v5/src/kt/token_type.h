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

#ifndef __KISHITECH_TOKEN_TYPE_H__
#define __KISHITECH_TOKEN_TYPE_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "debug.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define KT_TOKEN_TYPE_LIST \
	X_MACRO(KT_TOKEN_WORD) \
	X_MACRO(KT_TOKEN_VAR) \
	X_MACRO(KT_TOKEN_NUMBER) \
	X_MACRO(KT_TOKEN_WHITESPACE) \
	X_MACRO(KT_TOKEN_NEWLINE) \
	X_MACRO(KT_TOKEN_EOF) \
	X_MACRO(KT_TOKEN_EQUALS) \
	X_MACRO(KT_TOKEN_ADD) \
	X_MACRO(KT_TOKEN_SUB) \
	X_MACRO(KT_TOKEN_MUL) \
	X_MACRO(KT_TOKEN_DIV) \
	X_MACRO(KT_TOKEN_POW) \
	X_MACRO(KT_TOKEN_NEG) \
	X_MACRO(KT_TOKEN_OPEN_PAREN) \
	X_MACRO(KT_TOKEN_CLOSE_PAREN) \
	X_MACRO(KT_TOKEN_STMT_LET) \
	X_MACRO(KT_TOKEN_STMT_RESET) \
	X_MACRO(KT_TOKEN_STMT_VARS) \
	X_MACRO(KT_TOKEN_NOT_SUPPORTED) \
	X_MACRO(KT_TOKEN_ERROR) \

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
enum ktTokenType
{
#define X_MACRO(name) name,
	KT_TOKEN_TYPE_LIST
#undef X_MACRO

#if _DEBUG_RPN
	KT_TOKEN_STMT_RPN,
#endif // #if _DEBUG_RPN
};

typedef enum ktTokenType ktTokenType;

extern const char* const KT_TOKEN_TYPE_STR[];

#endif // __KISHITECH_TOKEN_TYPE_H__
