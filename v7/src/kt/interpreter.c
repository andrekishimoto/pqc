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
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "char_stack.h"
#include "double_stack.h"
#include "interpreter.h"
#include "memory.h"
#include "parser.h"
#include "token_symbols.h"
#include "consts.h"
#include "error_type.h"
#include "utils.h"

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
typedef struct ktInterpreter ktInterpreter;
typedef struct ktExpression ktExpression;

struct ktInterpreter
{
	bool isRunning;
	ktParserCallback* callback;
	ktMemory* memory;
	ktExpression* expression;
};

enum ktInterpreterConstants
{
	KT_EXPR_BUFFER_SIZE = 128,
	KT_EXPR_INPUT_MAX_LENGTH = 80,
};

struct ktExpression
{
	ktCharStack* symbolStack;
	char buffer[KT_EXPR_BUFFER_SIZE];
	size_t index;
	ktErrorType errorType;
};

//------------------------------------------------------------------------------
// Globals (argh!)
//------------------------------------------------------------------------------
static const char* const SOFTWARE_TITLE = "PQC";
static const char* const SOFTWARE_VERSION = "0.1";
static const char* const SOFTWARE_COPYRIGHT_YEAR = "2024";
static const char* const SOFTWARE_AUTHOR = "Andre Kishimoto";

static ktInterpreter* g_interpreter = NULL;

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
static void interpreterCreate(void);
static void interpreterDestroy(void);
static void interpreterExecute(const char* contents);

static void onLetStmt(int errorCode, char variable, double value);
static void onResetStmt(int errorCode);
static void onVarsStmt(int errorCode);
static void onClearStmt(void);
static void onExitStmt(void);
static void onExprStmtBegin(int errorCode);
static void onExprStmtEnd(int errorCode);
static void onVar(int errorCode, char variable);
static void onNumber(int errorCode, double number);
static void onSymbol(int errorCode, char symbol);
static void onError(ktErrorType errorType, const char* message);

static bool isOperator(char symbol);
static int operatorPriority(char operator);

static void exprBufferAppend(char value);
static void exprBufferReset(void);
static void exprBufferError(ktErrorType errorType);
static ktErrorType evaluateExpr(double* out_result);

static void printError(ktErrorType errorType);

#if _DEBUG_RPN
static void onRpnStmt(int errorCode);
#endif // #if _DEBUG_RPN

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void interpreterCreate(void)
{
	g_interpreter = malloc(sizeof(ktInterpreter));
	if (g_interpreter)
	{
		g_interpreter->callback = malloc(sizeof(ktParserCallback));
		if (g_interpreter->callback)
		{
			g_interpreter->callback->letStmt = onLetStmt;
			g_interpreter->callback->resetStmt = onResetStmt;
			g_interpreter->callback->varsStmt = onVarsStmt;
			g_interpreter->callback->clearStmt = onClearStmt;
			g_interpreter->callback->exitStmt = onExitStmt;
			g_interpreter->callback->exprStmtBegin = onExprStmtBegin;
			g_interpreter->callback->exprStmtEnd = onExprStmtEnd;
			g_interpreter->callback->var = onVar;
			g_interpreter->callback->number = onNumber;
			g_interpreter->callback->symbol = onSymbol;
			g_interpreter->callback->error = onError;

#if _DEBUG_RPN
			g_interpreter->callback->rpnStmt = onRpnStmt;
#endif // _DEBUG_RPN
		}

		ktParserCreate(g_interpreter->callback);
		g_interpreter->memory = ktMemoryCreate();
		g_interpreter->expression = malloc(sizeof(ktExpression));
		if (g_interpreter->expression)
		{
			g_interpreter->expression->symbolStack = ktCharStackCreate();
			memset(g_interpreter->expression->buffer, 0, KT_EXPR_BUFFER_SIZE);
		}

		g_interpreter->isRunning = true;
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void interpreterDestroy(void)
{
	if (g_interpreter)
	{
		g_interpreter->isRunning = false;
		SAFE_DELETE(g_interpreter->callback);
		ktParserDestroy();
		ktMemoryDestroy(g_interpreter->memory);
		ktCharStackDestroy(g_interpreter->expression->symbolStack);
		SAFE_DELETE(g_interpreter->expression);
		SAFE_DELETE(g_interpreter);
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktInterpreterRun(void)
{
	printf("%s v%s\nCopyright (c) %s %s.\n\n", SOFTWARE_TITLE, SOFTWARE_VERSION, SOFTWARE_COPYRIGHT_YEAR, SOFTWARE_AUTHOR);

	interpreterCreate();

	char str[KT_EXPR_INPUT_MAX_LENGTH] = { 0 };
	size_t strLength = 0;

	while (g_interpreter && g_interpreter->isRunning)
	{
		printf("> ");
		if (fgets(str, KT_EXPR_INPUT_MAX_LENGTH, stdin))
		{
			strLength = strlen(str);
			if (str[strLength - 1] == '\n')
			{
				str[strLength - 1] = '\0';
			}
			strLength = strlen(str);
			interpreterExecute(ktStringToUpper(str));
		}
	}

	interpreterDestroy();
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void interpreterExecute(const char* contents)
{
	if (!g_interpreter)
		return;

	ktParserRun(contents);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onLetStmt(int errorCode, char variable, double value)
{
	if ((errorCode & KT_LET_STMT_VAR_FLAG) == KT_LET_STMT_VAR_FLAG)
		printError(KT_ERROR_INTERPRETER_LET_STMT_VAR_NOT_SET);
	if ((errorCode & KT_LET_STMT_VALUE_FLAG) == KT_LET_STMT_VALUE_FLAG)
		printError(KT_ERROR_INTERPRETER_LET_STMT_VALUE_NOT_SET);
	if ((errorCode & KT_LET_STMT_PARAMS_FLAG) == KT_LET_STMT_PARAMS_FLAG)
		printError(KT_ERROR_INTERPRETER_LET_STMT_INVALID_PARAMS);
	if (errorCode)
		return;

	size_t index = (size_t)variable - (size_t)'A';
	ktMemorySet(g_interpreter->memory, index, value);
	printf("%c = %.*f\n", variable, DBL_DIG, g_interpreter->memory->vars[index]);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onResetStmt(int errorCode)
{
	if (errorCode)
	{
		printError(KT_ERROR_INTERPRETER_RESET_STMT_INVALID_PARAMS);
		return;
	}

	printf("Resetting all variables... ");
	ktMemoryReset(g_interpreter->memory);
	printf("Done.\n");
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onVarsStmt(int errorCode)
{
	if (errorCode)
	{
		printError(KT_ERROR_INTERPRETER_VARS_STMT_INVALID_PARAMS);
		return;
	}

	printf("Please note: The list below only displays variables that have an associated value.\n");

	size_t count = 0;
	for (size_t i = 0; i < KT_VAR_COUNT; ++i)
	{
		if (g_interpreter->memory->hasValue[i])
		{
			++count;
			printf("%c = %.*f\n", (char)(i + 'A'), DBL_DIG, g_interpreter->memory->vars[i]);
		}
	}

	if (count == 0)
	{
		printf("*** None of the variables available [A-Z] are set! ***\n");
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onClearStmt()
{
#if _WIN32
	system("cls");
#else
	system("clear");
#endif
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onExitStmt()
{
	g_interpreter->isRunning = false;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onExprStmtBegin(int errorCode)
{
	if (errorCode)
		return;

	exprBufferReset();
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onExprStmtEnd(int errorCode)
{
	if (errorCode)
	{
		exprBufferError(KT_ERROR_INTERPRETER_EXPR_STMT_GENERIC);
	}

	if (g_interpreter->expression->errorType == KT_ERROR_NONE)
	{
		while (!ktCharStackIsEmpty(g_interpreter->expression->symbolStack))
		{
			if (ktCharStackTop(g_interpreter->expression->symbolStack) == KT_TOKEN_OPEN_PAREN_SYMBOL)
			{
				exprBufferError(KT_ERROR_INTERPRETER_EXPR_STMT_OPEN_PAREN);
				break;
			}

			exprBufferAppend(ktCharStackPop(g_interpreter->expression->symbolStack));
		}
	}

#if _DEBUG_RPN
	onRpnStmt(0);
#endif // #if _DEBUG_RPN

	if (g_interpreter->expression->errorType == KT_ERROR_NONE)
	{
		double result = 0.0;
		ktErrorType errorType = evaluateExpr(&result);
		if (errorType == KT_ERROR_NONE)
		{
			printf("%.*f\n", DBL_DIG, result);
		}
		else
		{
			exprBufferError(errorType);
		}
	}

	// Ignore KT_ERROR_INTERPRETER_EXPR_STMT_VAR_NOT_SET because this error was
	// already printed inside onVar() function.
	if (g_interpreter->expression->errorType != KT_ERROR_NONE
		&& g_interpreter->expression->errorType != KT_ERROR_INTERPRETER_EXPR_STMT_VAR_NOT_SET)
	{
		printError(g_interpreter->expression->errorType);
	}

	exprBufferReset();
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onVar(int errorCode, char variable)
{
	if (errorCode)
		return;

	size_t index = (size_t)variable - (size_t)'A';
	if (g_interpreter->memory->hasValue[index])
	{
		exprBufferAppend(variable);
	}
	else
	{
		char buffer[KT_ERROR_MESSAGE_MAX_LENGTH] = { 0 };
		snprintf(buffer, KT_ERROR_MESSAGE_MAX_LENGTH, ktErrorDescription(KT_ERROR_INTERPRETER_EXPR_STMT_VAR_NOT_SET), variable);
		onError(KT_ERROR_INTERPRETER_EXPR_STMT_VAR_NOT_SET, buffer);
		
		exprBufferError(KT_ERROR_INTERPRETER_EXPR_STMT_VAR_NOT_SET);
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onNumber(int errorCode, double number)
{
	printf("[callback] onNumber(errorCode: %d, number: %f)\n", errorCode, number);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onSymbol(int errorCode, char symbol)
{
	if (errorCode)
		return;

	if (symbol == KT_TOKEN_OPEN_PAREN_SYMBOL)
	{
		ktCharStackPush(g_interpreter->expression->symbolStack, symbol);
	}
	else if (symbol == KT_TOKEN_CLOSE_PAREN_SYMBOL)
	{
		while (ktCharStackTop(g_interpreter->expression->symbolStack) != KT_TOKEN_OPEN_PAREN_SYMBOL)
		{
			exprBufferAppend(ktCharStackPop(g_interpreter->expression->symbolStack));
		}

		// Discard KT_TOKEN_OPEN_PAREN_SYMBOL from the stack.
		ktCharStackPop(g_interpreter->expression->symbolStack);
	}
	else if (isOperator(symbol))
	{
		while (!ktCharStackIsEmpty(g_interpreter->expression->symbolStack)
			&& operatorPriority(symbol) <= operatorPriority(ktCharStackTop(g_interpreter->expression->symbolStack)))
		{
			exprBufferAppend(ktCharStackPop(g_interpreter->expression->symbolStack));
		}

		ktCharStackPush(g_interpreter->expression->symbolStack, symbol);
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onError(ktErrorType errorType, const char* message)
{
	// Don't print KT_ERROR_PARSER_CONSUME_EXPECTED_GOT in the final build.
	if (errorType == KT_ERROR_PARSER_CONSUME_EXPECTED_GOT)
		return;

	printf("*** ERROR: (%d) %s\n", errorType, message);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool isOperator(char symbol)
{
	return symbol == KT_TOKEN_NEG_SYMBOL
		|| symbol == KT_TOKEN_POW_SYMBOL
		|| symbol == KT_TOKEN_DIV_SYMBOL
		|| symbol == KT_TOKEN_MUL_SYMBOL
		|| symbol == KT_TOKEN_SUB_SYMBOL
		|| symbol == KT_TOKEN_ADD_SYMBOL;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int operatorPriority(char operator)
{
	if (operator == KT_TOKEN_NEG_SYMBOL)
		return 4;
	else if (operator == KT_TOKEN_POW_SYMBOL)
		return 3;
	else if (operator == KT_TOKEN_MUL_SYMBOL || operator == KT_TOKEN_DIV_SYMBOL)
		return 2;
	else if (operator == KT_TOKEN_ADD_SYMBOL || operator == KT_TOKEN_SUB_SYMBOL)
		return 1;
	else
		return 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void exprBufferAppend(char value)
{
	if (g_interpreter->expression->index + 1 >= KT_EXPR_BUFFER_SIZE)
	{
		exprBufferError(KT_ERROR_INTERPRETER_EXPR_STMT_BUFFER_OVERFLOW);
		return;
	}

	g_interpreter->expression->buffer[g_interpreter->expression->index++] = value;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void exprBufferReset(void)
{
	memset(g_interpreter->expression->buffer, 0, KT_EXPR_BUFFER_SIZE);
	g_interpreter->expression->index = 0;
	g_interpreter->expression->errorType = KT_ERROR_NONE;
	ktCharStackClear(g_interpreter->expression->symbolStack);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void exprBufferError(ktErrorType errorType)
{
	g_interpreter->expression->errorType = errorType;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ktErrorType evaluateExpr(double *out_result)
{
	*out_result = 0.0;
	ktDoubleStack stack =
	{
		.data = { 0 },
		.count = 0
	};

	const char* expr = g_interpreter->expression->buffer;
	size_t exprLength = strlen(expr);
	for (size_t i = 0; i < exprLength; ++i)
	{
		if (isOperator(expr[i]))
		{
			if (ktDoubleStackIsEmpty(&stack))
				return KT_ERROR_INTERPRETER_EXPR_STMT_EXTRA_OPERATOR;

			double result = 0.0;

			if (expr[i] == KT_TOKEN_NEG_SYMBOL)
			{
				result = -ktDoubleStackPop(&stack);
			}
			else
			{
				double n2 = ktDoubleStackPop(&stack);

				if (ktDoubleStackIsEmpty(&stack))
					return KT_ERROR_INTERPRETER_EXPR_STMT_MISSING_OPERAND;

				double n1 = ktDoubleStackPop(&stack);

				if (expr[i] == KT_TOKEN_ADD_SYMBOL)
				{
					result = n1 + n2;
				}
				else if (expr[i] == KT_TOKEN_SUB_SYMBOL)
				{
					result = n1 - n2;
				}
				else if (expr[i] == KT_TOKEN_MUL_SYMBOL)
				{
					result = n1 * n2;
				}
				else if (expr[i] == KT_TOKEN_DIV_SYMBOL)
				{
					if (fabs(n2) < DBL_EPSILON)
						return KT_ERROR_INTERPRETER_EXPR_STMT_DIV_BY_ZERO;

					result = n1 / n2;
				}
				else if (expr[i] == KT_TOKEN_POW_SYMBOL)
				{
					result = pow(n1, n2);
				}
			}

			ktDoubleStackPush(&stack, result);
		}
		else
		{
			size_t index = (size_t)expr[i] - (size_t)'A';
			ktDoubleStackPush(&stack, g_interpreter->memory->vars[index]);
		}
	}

	if (!ktDoubleStackIsEmpty(&stack))
	{
		*out_result = ktDoubleStackPop(&stack);
	}

	return ktDoubleStackIsEmpty(&stack) ? KT_ERROR_NONE : KT_ERROR_INTERPRETER_EXPR_STMT_GENERIC;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void printError(ktErrorType errorType)
{
	onError(errorType, ktErrorDescription(errorType));
}

#if _DEBUG_RPN
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onRpnStmt(int errorCode)
{
	if (errorCode)
		return;

	printf("RPN: %s\n", g_interpreter->expression->buffer);
	printf("symbolStack: %s\n", g_interpreter->expression->symbolStack->data);
}
#endif // #if _DEBUG_RPN
