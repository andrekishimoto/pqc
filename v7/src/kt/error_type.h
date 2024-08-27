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

#ifndef __KISHITECH_ERROR_H__
#define __KISHITECH_ERROR_H__

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define KT_ERROR_TYPE_LIST \
	X_MACRO(KT_ERROR_NONE) \
	X_MACRO(KT_ERROR_TOKENIZER_EXTRA_DECIMAL_POINT) \
	X_MACRO(KT_ERROR_TOKENIZER_STR_TO_NUMBER) \
	X_MACRO(KT_ERROR_TOKENIZER_INVALID_TOKEN) \
	X_MACRO(KT_ERROR_PARSER_UNKNOWN_COMMAND) \
	X_MACRO(KT_ERROR_PARSER_DID_YOU_MEAN_LET) \
	X_MACRO(KT_ERROR_PARSER_NO_MORE_TOKENS) \
	X_MACRO(KT_ERROR_PARSER_TOKENIZER_ERROR) \
	X_MACRO(KT_ERROR_PARSER_CONSUME_EXPECTED_GOT) \
	X_MACRO(KT_ERROR_INTERPRETER_LET_STMT_VAR_NOT_SET) \
	X_MACRO(KT_ERROR_INTERPRETER_LET_STMT_VALUE_NOT_SET) \
	X_MACRO(KT_ERROR_INTERPRETER_LET_STMT_INVALID_PARAMS) \
	X_MACRO(KT_ERROR_INTERPRETER_RESET_STMT_INVALID_PARAMS) \
	X_MACRO(KT_ERROR_INTERPRETER_VARS_STMT_INVALID_PARAMS) \
	X_MACRO(KT_ERROR_INTERPRETER_EXPR_STMT_GENERIC) \
	X_MACRO(KT_ERROR_INTERPRETER_EXPR_STMT_BUFFER_OVERFLOW) \
	X_MACRO(KT_ERROR_INTERPRETER_EXPR_STMT_OPEN_PAREN) \
	X_MACRO(KT_ERROR_INTERPRETER_EXPR_STMT_EXTRA_OPERATOR) \
	X_MACRO(KT_ERROR_INTERPRETER_EXPR_STMT_MISSING_OPERAND) \
	X_MACRO(KT_ERROR_INTERPRETER_EXPR_STMT_VAR_NOT_SET) \
	X_MACRO(KT_ERROR_INTERPRETER_EXPR_STMT_DIV_BY_ZERO)

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
enum ktErrorType
{
#define X_MACRO(name) name,
	KT_ERROR_TYPE_LIST
#undef X_MACRO
};

typedef enum ktErrorType ktErrorType;

extern const char* const KT_ERROR_TYPE_STR[];

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
const char* ktErrorDescription(ktErrorType type);

#endif // __KISHITECH_ERROR_H__
