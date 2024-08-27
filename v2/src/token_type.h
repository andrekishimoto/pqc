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
// Custom types, structs, etc.
//------------------------------------------------------------------------------
enum TokenType
{
	TOKEN_WORD,
	TOKEN_VAR,
	TOKEN_NUMBER,
	TOKEN_WHITESPACE,
	TOKEN_NEWLINE,
	TOKEN_EOF,
	TOKEN_EQUALS,
	TOKEN_ADD,
	TOKEN_SUB,
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_POW,
	TOKEN_NEG,
	TOKEN_OPEN_PAREN,
	TOKEN_CLOSE_PAREN,
	TOKEN_STMT_LET,
	TOKEN_STMT_RESET,
	TOKEN_STMT_VARS,
	TOKEN_ERROR,
};

typedef enum TokenType TokenType;

#endif // __KISHITECH_TOKEN_TYPE_H__
