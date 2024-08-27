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
#include "error_type.h"

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
const char* const KT_ERROR_TYPE_STR[] =
{
#define X_MACRO(name) #name,
	KT_ERROR_TYPE_LIST
#undef X_MACRO
};

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
const char* ktErrorDescription(ktErrorType type)
{
	switch (type)
	{
	default:
	case KT_ERROR_NONE:
		return "";

	case KT_ERROR_TOKENIZER_STR_TO_NUMBER:
		return "Could not convert '%s' to number.";

	case KT_ERROR_TOKENIZER_INVALID_TOKEN:
		return "Invalid token '%c'.";

	case KT_ERROR_PARSER_UNKNOWN_COMMAND:
		return "Unknown command '%s'.";

	case KT_ERROR_PARSER_NO_MORE_TOKENS:
		return "There are no more tokens to parse (unexpected KT_TOKEN_EOF).";

	case KT_ERROR_PARSER_CONSUME_EXPECTED_GOT:
		return "Incorrect token. Expected: %s, got: %s.";

	case KT_ERROR_INTERPRETER_LET_STMT_VAR_NOT_SET:
		return "Variable not set (valid variables are [A-Z]).";
	
	case KT_ERROR_INTERPRETER_LET_STMT_VALUE_NOT_SET:
		return "Value not set (only numbers are considered values).";
	
	case KT_ERROR_INTERPRETER_LET_STMT_INVALID_PARAMS:
		return "LET only accepts one value to be assigned to a single variable.";

	case KT_ERROR_INTERPRETER_RESET_STMT_INVALID_PARAMS:
		return "RESET does not accept parameters.";

	case KT_ERROR_INTERPRETER_VARS_STMT_INVALID_PARAMS:
		return "VARS does not accept parameters.";

	case KT_ERROR_INTERPRETER_EXPR_STMT_GENERIC:
		return "The expression has one or more errors.";

	case KT_ERROR_INTERPRETER_EXPR_STMT_BUFFER_OVERFLOW:
		return "Expression buffer overflow.";
			
	case KT_ERROR_INTERPRETER_EXPR_STMT_OPEN_PAREN:
		return "The expression contains an opened parenthesis.";

	case KT_ERROR_INTERPRETER_EXPR_STMT_EXTRA_OPERATOR:
		return "Invalid extra operator.";

	case KT_ERROR_INTERPRETER_EXPR_STMT_MISSING_OPERAND:
		return "Missing operand.";

	case KT_ERROR_INTERPRETER_EXPR_STMT_VAR_NOT_SET:
		return "One or more variables are not set.";

	case KT_ERROR_INTERPRETER_EXPR_STMT_DIV_BY_ZERO:
		return "Divide by zero.";
	}
}
