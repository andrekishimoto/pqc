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
#include "token_symbols.h"

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
const char TOKEN_WHITESPACE_SYMBOL = ' ';
const char TOKEN_NEWLINE_SYMBOL = '\n';
const char TOKEN_EOF_SYMBOL = '\0';
const char TOKEN_EQUALS_SYMBOL = '=';
const char TOKEN_ADD_SYMBOL = '+';
const char TOKEN_SUB_SYMBOL = '-';
const char TOKEN_MUL_SYMBOL = '*';
const char TOKEN_DIV_SYMBOL = '/';
const char TOKEN_POW_SYMBOL = '^';
const char TOKEN_NEG_SYMBOL = '~'; // Negation uses a different symbol.
const char TOKEN_OPEN_PAREN_SYMBOL = '(';
const char TOKEN_CLOSE_PAREN_SYMBOL = ')';

const char* const TOKEN_STMT_LET_VALUE = "LET";
const char* const TOKEN_STMT_RESET_VALUE = "RESET";
const char* const TOKEN_STMT_VARS_VALUE = "VARS";
const char* const TOKEN_STMT_RPN_VALUE = "RPN"; // TODO: *** DEBUG CODE - REMOVE ME FROM FINAL BUILD. ***
