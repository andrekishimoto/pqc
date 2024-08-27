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

#ifndef __KISHITECH_TOKEN_SYMBOLS_H__
#define __KISHITECH_TOKEN_SYMBOLS_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "debug.h"

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
extern const char KT_TOKEN_WHITESPACE_SYMBOL;
extern const char KT_TOKEN_NEWLINE_SYMBOL;
extern const char KT_TOKEN_EOF_SYMBOL;
extern const char KT_TOKEN_EQUALS_SYMBOL;
extern const char KT_TOKEN_ADD_SYMBOL;
extern const char KT_TOKEN_SUB_SYMBOL;
extern const char KT_TOKEN_MUL_SYMBOL;
extern const char KT_TOKEN_DIV_SYMBOL;
extern const char KT_TOKEN_POW_SYMBOL;
extern const char KT_TOKEN_NEG_SYMBOL;
extern const char KT_TOKEN_OPEN_PAREN_SYMBOL;
extern const char KT_TOKEN_CLOSE_PAREN_SYMBOL;

extern const char* const KT_TOKEN_STMT_LET_VALUE;
extern const char* const KT_TOKEN_STMT_RESET_VALUE;
extern const char* const KT_TOKEN_STMT_VARS_VALUE;

#if _DEBUG_RPN
extern const char* const KT_TOKEN_STMT_RPN_VALUE;
#endif // #if _DEBUG_RPN

#endif // __KISHITECH_TOKEN_SYMBOLS_H__
