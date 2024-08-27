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
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "tokenizer.h"
#include "token_symbols.h"
#include "utils.h"
#include "error_type.h"
#include "debug.h"

//------------------------------------------------------------------------------
// Globals (argh!)
//------------------------------------------------------------------------------
static const char* data = NULL;
static unsigned char curr = '\0';
static int length = 0;
static int index = 0;

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
static void reset();
static void advance();
static void retreat();
static unsigned char peek(size_t ahead);

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenizerRun(const char* contents, ktTokenList* out_list)
{
	if (!out_list)
		return;

	ktTokenListClear(out_list);

	data = contents;
	length = (int)strlen(data);

	reset();

	while (true)
	{
		if (index >= length)
		{
			// HACK: Adding a KT_TOKEN_NEWLINE before KT_TOKEN_EOF because our grammar
			// requires a line break after every valid statement. If we are calling
			// the parser from our own REPL (contents come from fgets(stdin)), then
			// we are probably changing the '\n' (added via fgets) to '\0'. If that
			// is the case, then there is no line break after a statement.
			ktTokenListAppend(out_list, ktTokenCreateSymbol(KT_TOKEN_NEWLINE));

			ktTokenListAppend(out_list, ktTokenCreateSymbol(KT_TOKEN_EOF));
			break;
		}

		advance();

		if (isblank(curr))
		{
			// Skip all whitespaces.
			while (isblank(curr))
			{
				advance();
			}

			// Go back one character so the first advance() call in the next
			// loop iteration gets the correct character. This must be done
			// because we always advance to the next character in the loop
			// above that is used to skip blank characters.
			retreat();
		}
		else if (curr == '\n' || curr == '\r')
		{
			// Handle the case where "\r\n" is used for a newline.
			if (curr == '\r' && peek(1) == '\n')
			{
				advance();
			}
			ktTokenListAppend(out_list, ktTokenCreateSymbol(KT_TOKEN_NEWLINE));
		}
		else if (curr == KT_TOKEN_EQUALS_SYMBOL)
		{
			ktTokenListAppend(out_list, ktTokenCreateSymbol(KT_TOKEN_EQUALS));
		}
		else if (curr == KT_TOKEN_ADD_SYMBOL)
		{
			ktTokenListAppend(out_list, ktTokenCreateSymbol(KT_TOKEN_ADD));
		}
		else if (curr == KT_TOKEN_SUB_SYMBOL)
		{
			ktTokenType previousTokenType = ktTokenListIsEmpty(out_list) ? KT_TOKEN_EOF : out_list->tail->token->type;

			bool isNegate = previousTokenType == KT_TOKEN_EOF
				|| previousTokenType == KT_TOKEN_NEWLINE
				|| previousTokenType == KT_TOKEN_EQUALS
				|| previousTokenType == KT_TOKEN_ADD
				|| previousTokenType == KT_TOKEN_SUB
				|| previousTokenType == KT_TOKEN_MUL
				|| previousTokenType == KT_TOKEN_DIV
				|| previousTokenType == KT_TOKEN_POW
				|| previousTokenType == KT_TOKEN_NEG
				|| previousTokenType == KT_TOKEN_OPEN_PAREN;
			
			// If we are adding a KT_TOKEN_NEG and the last token in the list is KT_TOKEN_NEG,
			// just remove the last token (KT_TOKEN_NEG) from the list and don't add anything,
			// since a sequence of KT_TOKEN_NEG KT_TOKEN_NEG will cancel each other out.
			if (isNegate && previousTokenType == KT_TOKEN_NEG)
			{
				ktTokenListRemoveTail(out_list);
			}
			else
			{
				ktTokenListAppend(out_list, ktTokenCreateSymbol(isNegate ? KT_TOKEN_NEG : KT_TOKEN_SUB));
			}
		}
		else if (curr == KT_TOKEN_MUL_SYMBOL)
		{
			ktTokenListAppend(out_list, ktTokenCreateSymbol(KT_TOKEN_MUL));
		}
		else if (curr == KT_TOKEN_DIV_SYMBOL)
		{
			ktTokenListAppend(out_list, ktTokenCreateSymbol(KT_TOKEN_DIV));
		}
		else if (curr == KT_TOKEN_POW_SYMBOL)
		{
			ktTokenListAppend(out_list, ktTokenCreateSymbol(KT_TOKEN_POW));
		}
		else if (curr == KT_TOKEN_OPEN_PAREN_SYMBOL)
		{
			ktTokenListAppend(out_list, ktTokenCreateSymbol(KT_TOKEN_OPEN_PAREN));
		}
		else if (curr == KT_TOKEN_CLOSE_PAREN_SYMBOL)
		{
			ktTokenListAppend(out_list, ktTokenCreateSymbol(KT_TOKEN_CLOSE_PAREN));
		}
		else if (isdigit(curr) || curr == '.')
		{
			bool isDouble = (curr == '.');
			bool hasError = false; // TODO: Rewrite? Maybe "throw" an exception, raise an error?

			size_t concatStartIndex = index;
			while (isdigit(curr) || curr == '.')
			{
				advance();
				if (curr == '.')
				{
					if (!isDouble)
					{
						isDouble = true;
					}
					else
					{
						// ERROR: There is an extra decimal point in the expression.
						advance(); // TODO: See TODO above; for now, simply ignore this extra decimal point.
						hasError = true;
						break;
					}
				}
			}
			size_t concatEndIndex = hasError ? index - 2 : index - 1; // TODO: Rewrite (see TODO above).

			retreat();

			char* numberStr = NULL;
			ktStringCopyInterval(&numberStr, data, concatStartIndex, concatEndIndex);

			char* endPtr = NULL;
			double number = strtod(numberStr, &endPtr);

			// Successful string to double conversion.
			if (*endPtr == KT_TOKEN_EOF_SYMBOL)
			{
				ktTokenListAppend(out_list, ktTokenCreateNumber(number));
			}
			else
			{
				char errorMsg[64] = { 0 };
				snprintf(errorMsg, 64, ktErrorDescription(KT_ERROR_TOKENIZER_STR_TO_NUMBER), numberStr);
				ktTokenListAppend(out_list, ktTokenCreateError(errorMsg));
			}

			ktStringDestroy(&numberStr);
		}
		else if (isalpha(curr) || ispunct(curr))
		{
			// Only one letter followed by a whitespace or any valid symbol - we have found a variable!
			unsigned char ahead = peek(1);
			if (!ispunct(curr) && (isblank(ahead)
				|| ahead == KT_TOKEN_NEWLINE_SYMBOL
				|| ahead == KT_TOKEN_EOF_SYMBOL
				|| ahead == KT_TOKEN_EQUALS_SYMBOL
				|| ahead == KT_TOKEN_ADD_SYMBOL
				|| ahead == KT_TOKEN_SUB_SYMBOL
				|| ahead == KT_TOKEN_MUL_SYMBOL
				|| ahead == KT_TOKEN_DIV_SYMBOL
				|| ahead == KT_TOKEN_POW_SYMBOL
				|| ahead == KT_TOKEN_OPEN_PAREN_SYMBOL
				|| ahead == KT_TOKEN_CLOSE_PAREN_SYMBOL))
			{
				ktTokenListAppend(out_list, ktTokenCreateVar(curr));
			}
			else // We are inside a string - check for single word and/or valid statements.
			{
				size_t concatStartIndex = index;
				while (!isblank(curr) && curr != KT_TOKEN_EOF_SYMBOL)
				{
					advance();
				}
				size_t concatEndIndex = ktMax((size_t)index - 1, 0);

				retreat();

				char* str = NULL;
				ktStringCopyInterval(&str, data, concatStartIndex, concatEndIndex);

				size_t strLength = strlen(str);
				size_t letLength = ktMax(strLength, strlen(KT_TOKEN_STMT_LET_VALUE));
				size_t resetLength = ktMax(strLength, strlen(KT_TOKEN_STMT_RESET_VALUE));
				size_t varsLength = ktMax(strLength, strlen(KT_TOKEN_STMT_VARS_VALUE));

#if _DEBUG_RPN
				size_t rpnLength = ktMax(strLength, strlen(KT_TOKEN_STMT_RPN_VALUE));
#endif // #if _DEBUG_RPN

				if (strncmp(str, KT_TOKEN_STMT_LET_VALUE, letLength) == 0)
				{
					ktTokenListAppend(out_list, ktTokenCreateStmtLet());
				}
				else if (strncmp(str, KT_TOKEN_STMT_RESET_VALUE, resetLength) == 0)
				{
					ktTokenListAppend(out_list, ktTokenCreateStmtReset());
				}
				else if (strncmp(str, KT_TOKEN_STMT_VARS_VALUE, varsLength) == 0)
				{
					ktTokenListAppend(out_list, ktTokenCreateStmtVars());
				}

#if _DEBUG_RPN
				else if (strncmp(str, KT_TOKEN_STMT_RPN_VALUE, rpnLength) == 0)
				{
					ktTokenListAppend(out_list, ktTokenCreateStmtRpn());
				}
#endif // #if _DEBUG_RPN

				else
				{
					// For now, we only recognize single words separated by
					// spaces. Later, we should add support for strings
					// (i.e., one or more words grouped together).
					ktTokenListAppend(out_list, ktTokenCreateWord(str));
				}

				ktStringDestroy(&str);
			}
		}
		else
		{
			if (curr != KT_TOKEN_EOF_SYMBOL)
			{
				//ktTokenListAppend(out_list, ktTokenCreateNotSupported(curr));

				char errorMsg[64] = { 0 };
				snprintf(errorMsg, 64, ktErrorDescription(KT_ERROR_TOKENIZER_INVALID_TOKEN), curr);
				ktTokenListAppend(out_list, ktTokenCreateError(errorMsg));
			}
		}
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void reset()
{
	index = -1;
	curr = '\0';
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void advance()
{
	++index;
	curr = (index < length) ? data[index] : '\0';
}

//------------------------------------------------------------------------------
// The function retreat() is mainly used after loops that advance to the next
// character until a condition is met (e.g. skip whitespaces, get an interval
// of characters inside a string).
// In these cases, we want to go back to the previous character so the next
// advance() call gets the correct character in the sequence.
//------------------------------------------------------------------------------
void retreat()
{
	--index;
	curr = (index > -1) ? data[index] : '\0';
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
unsigned char peek(size_t offset)
{
	return (index + (int)offset < length) ? data[index + offset] : '\0';
}

