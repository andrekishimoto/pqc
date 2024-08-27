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
const char TOKEN_SYMBOL_EQUALS = '=';
const char TOKEN_SYMBOL_ADD = '+';
const char TOKEN_SYMBOL_SUB = '-';
const char TOKEN_SYMBOL_MUL = '*';
const char TOKEN_SYMBOL_DIV = '/';
const char TOKEN_SYMBOL_POW = '^';
const char TOKEN_SYMBOL_OPEN_PAREN = '(';
const char TOKEN_SYMBOL_CLOSE_PAREN = ')';

const char* const TOKEN_STMT_LET_VALUE = "LET";
const char* const TOKEN_STMT_RESET_VALUE = "RESET";
const char* const TOKEN_STMT_VARS_VALUE = "VARS";
