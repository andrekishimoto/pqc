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
// Macros
//------------------------------------------------------------------------------
#define TOKEN_TYPE_LIST \
	X_MACRO(TOKEN_WORD) \
	X_MACRO(TOKEN_VAR) \
	X_MACRO(TOKEN_NUMBER) \
	X_MACRO(TOKEN_WHITESPACE) \
	X_MACRO(TOKEN_NEWLINE) \
	X_MACRO(TOKEN_EOF) \
	X_MACRO(TOKEN_EQUALS) \
	X_MACRO(TOKEN_ADD) \
	X_MACRO(TOKEN_SUB) \
	X_MACRO(TOKEN_MUL) \
	X_MACRO(TOKEN_DIV) \
	X_MACRO(TOKEN_POW) \
	X_MACRO(TOKEN_NEG) \
	X_MACRO(TOKEN_OPEN_PAREN) \
	X_MACRO(TOKEN_CLOSE_PAREN) \
	X_MACRO(TOKEN_STMT_LET) \
	X_MACRO(TOKEN_STMT_RESET) \
	X_MACRO(TOKEN_STMT_VARS) \
	X_MACRO(TOKEN_ERROR)

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
enum TokenType
{
#define X_MACRO(name) name,
	TOKEN_TYPE_LIST
#undef X_MACRO
};

typedef enum TokenType TokenType;

extern const char* const TOKEN_TYPE_STR[];

#endif // __KISHITECH_TOKEN_TYPE_H__
