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
// Custom types, structs, etc.
//------------------------------------------------------------------------------
extern const char TOKEN_WHITESPACE_SYMBOL;
extern const char TOKEN_NEWLINE_SYMBOL;
extern const char TOKEN_EOF_SYMBOL;
extern const char TOKEN_EQUALS_SYMBOL;
extern const char TOKEN_ADD_SYMBOL;
extern const char TOKEN_SUB_SYMBOL;
extern const char TOKEN_MUL_SYMBOL;
extern const char TOKEN_DIV_SYMBOL;
extern const char TOKEN_POW_SYMBOL;
extern const char TOKEN_NEG_SYMBOL;
extern const char TOKEN_OPEN_PAREN_SYMBOL;
extern const char TOKEN_CLOSE_PAREN_SYMBOL;

extern const char* const TOKEN_STMT_LET_VALUE;
extern const char* const TOKEN_STMT_RESET_VALUE;
extern const char* const TOKEN_STMT_VARS_VALUE;
extern const char* const TOKEN_STMT_RPN_VALUE; // TODO: *** DEBUG CODE - REMOVE ME FROM FINAL BUILD. ***

#endif // __KISHITECH_TOKEN_SYMBOLS_H__
