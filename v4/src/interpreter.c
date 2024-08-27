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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "parser.h"
#include "memory.h"
#include "token_symbols.h"
#include "char_stack.h"
#include "float_stack.h"
#include "utils.h"

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
typedef struct Interpreter Interpreter;
typedef struct Expression Expression;

struct Interpreter
{
	Parser* parser;
	ParserCallback* callback;
	Memory* memory;
	Expression* expression;
};

enum InterpreterConstants
{
	EXPR_BUFFER_SIZE = 128,
};

struct Expression
{
	CharStack* symbolStack;
	char buffer[EXPR_BUFFER_SIZE];
	size_t index;
	bool hasError;
};

//------------------------------------------------------------------------------
// Globals (argh!)
//------------------------------------------------------------------------------
static Interpreter* g_interpreter = NULL;

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
void onLetStmt(int errorCode, char variable, float value);
void onResetStmt(int errorCode);
void onVarsStmt(int errorCode);
void onExprStmtBegin(int errorCode);
void onExprStmtEnd(int errorCode);
void onRpnStmt(int errorCode); // // TODO: *** DEBUG CODE - REMOVE ME FROM FINAL BUILD. ***
void onVar(int errorCode, char variable);
void onNumber(int errorCode, float number);
void onSymbol(int errorCode, char symbol);
void onError(int errorCode, const char* message);

bool isOperator(char symbol);
int operatorPriority(char operator);

void exprBufferAppend(char value);
void exprBufferReset();
void exprBufferError();
bool evaluateExpr(float* out_result);

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktInterpreterCreate()
{
	g_interpreter = malloc(sizeof(Interpreter));
	if (g_interpreter)
	{
		g_interpreter->callback = malloc(sizeof(ParserCallback));
		if (g_interpreter->callback)
		{
			g_interpreter->callback->letStmt = onLetStmt;
			g_interpreter->callback->resetStmt = onResetStmt;
			g_interpreter->callback->varsStmt = onVarsStmt;
			g_interpreter->callback->exprStmtBegin = onExprStmtBegin;
			g_interpreter->callback->exprStmtEnd = onExprStmtEnd;
			g_interpreter->callback->var = onVar;
			g_interpreter->callback->number = onNumber;
			g_interpreter->callback->symbol = onSymbol;
			g_interpreter->callback->error = onError;

			g_interpreter->callback->rpnStmt = onRpnStmt; // TODO: *** DEBUG CODE - REMOVE ME FROM FINAL BUILD. ***
		}

		g_interpreter->parser = ktParserCreate(g_interpreter->callback);
		g_interpreter->memory = ktMemoryCreate();
		g_interpreter->expression = malloc(sizeof(Expression));
		if (g_interpreter->expression)
		{
			g_interpreter->expression->symbolStack = ktCharStackCreate();
			memset(g_interpreter->expression->buffer, 0, EXPR_BUFFER_SIZE);
		}

	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktInterpreterDestroy()
{
	if (g_interpreter)
	{
		SAFE_DELETE(g_interpreter->callback);
		ktParserDestroy(g_interpreter->parser);
		ktMemoryDestroy(g_interpreter->memory);
		ktCharStackDestroy(g_interpreter->expression->symbolStack);
		SAFE_DELETE(g_interpreter->expression);
		SAFE_DELETE(g_interpreter);
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktInterpreterRun(const char* contents)
{
	if (!g_interpreter)
		return;

	ktParserRun(g_interpreter->parser, contents);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onLetStmt(int errorCode, char variable, float value)
{
	//printf("[callback] onLetStmt(errorCode: %d, variable: '%c', value: %f)\n", errorCode, variable, value);
	
	if ((errorCode & 0x01) == 0x01)
	{
		printf("*** ERROR: Variable not set.\n");
	}

	if ((errorCode & 0x02) == 0x02)
	{
		printf("*** ERROR: Value not set.\n");
	}

	if (errorCode)
		return;

	size_t index = (size_t)variable - (size_t)'A';
	ktMemorySet(g_interpreter->memory, index, value);
	printf("%c = %f\n", variable, g_interpreter->memory->vars[index]);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onResetStmt(int errorCode)
{
	//printf("[callback] onResetStmt(errorCode: %d)\n", errorCode);

	if (errorCode)
		return;

	printf("Resetting all variables... ");
	ktMemoryReset(g_interpreter->memory);
	printf("Done.\n");
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onVarsStmt(int errorCode)
{
	//printf("[callback] onVarsStmt(errorCode: %d)\n", errorCode);

	if (errorCode)
		return;

	printf("Please note: The list below only displays variables that have an associated value.\n");

	size_t count = 0;
	for (size_t i = 0; i < VAR_COUNT; ++i)
	{
		if (g_interpreter->memory->hasValue[i])
		{
			++count;
			printf("%c = %f\n", (char)(i + 'A'), g_interpreter->memory->vars[i]);
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
void onExprStmtBegin(int errorCode)
{
	//printf("[callback] onExprStmtBegin(errorCode: %d)\n", errorCode);

	if (errorCode)
		return;

	exprBufferReset();
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onExprStmtEnd(int errorCode)
{
	//printf("[callback] onExprStmtEnd(errorCode: %d)\n", errorCode);

	if (errorCode)
	{
		printf("*** ERROR: The expression has one or more errors.\n");
		//onRpnStmt(0);
		exprBufferReset();
		return;
	}

	while (!ktCharStackIsEmpty(g_interpreter->expression->symbolStack))
	{
		if (ktCharStackTop(g_interpreter->expression->symbolStack) == TOKEN_OPEN_PAREN_SYMBOL)
		{
			printf("*** ERROR: The expression contains an opened parenthesis.\n");
			exprBufferError();
			return;
		}

		exprBufferAppend(ktCharStackPop(g_interpreter->expression->symbolStack));
	}

	if (g_interpreter->expression->hasError)
	{
		exprBufferReset();
	}
	else
	{
		onRpnStmt(0);
		

		float result = 0.0f;
		if (evaluateExpr(&result))
		{
			printf("%f\n", result);
		}
		else
		{
			printf("*** ERROR: The expression has one or more errors.\n");
		}
		exprBufferReset();
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onVar(int errorCode, char variable)
{
	//printf("[callback] onVar(errorCode: %d, variable: %c)\n", errorCode, variable);

	if (errorCode)
		return;

	size_t index = (size_t)variable - (size_t)'A';
	if (g_interpreter->memory->hasValue[index])
	{
		//printf("%c = %f\n", variable, g_interpreter->memory->vars[index]);
		exprBufferAppend(variable);
	}
	else
	{
		printf("*** ERROR: Variable %c is not set.\n", variable);
		exprBufferError();
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onNumber(int errorCode, float number)
{
	//printf("[callback] onNumber(errorCode: %d, number: %f)\n", errorCode, number);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void onSymbol(int errorCode, char symbol)
{
	//printf("[callback] onSymbol(errorCode: %d, symbol: \'%c\')\n", errorCode, symbol);
	
	if (symbol == TOKEN_OPEN_PAREN_SYMBOL)
	{
		ktCharStackPush(g_interpreter->expression->symbolStack, symbol);
	}
	else if (symbol == TOKEN_CLOSE_PAREN_SYMBOL)
	{
		while (ktCharStackTop(g_interpreter->expression->symbolStack) != TOKEN_OPEN_PAREN_SYMBOL)
		{
			exprBufferAppend(ktCharStackPop(g_interpreter->expression->symbolStack));
		}

		// Discard TOKEN_OPEN_PAREN_SYMBOL from the stack.
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
void onError(int errorCode, const char* message)
{
	//printf("[callback] onError(errorCode: %d, message: \"%s\")\n", errorCode, message);

	if (errorCode == 1)
		return;

	printf("*** ERROR: (%d) %s\n", errorCode, message);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool isOperator(char symbol)
{
	return symbol == TOKEN_NEG_SYMBOL
		|| symbol == TOKEN_POW_SYMBOL
		|| symbol == TOKEN_DIV_SYMBOL
		|| symbol == TOKEN_MUL_SYMBOL
		|| symbol == TOKEN_SUB_SYMBOL
		|| symbol == TOKEN_ADD_SYMBOL;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int operatorPriority(char operator)
{
	if (operator == TOKEN_NEG_SYMBOL)
	{
		return 4;
	}
	if (operator == TOKEN_POW_SYMBOL)
	{
		return 3;
	}
	else if (operator == TOKEN_MUL_SYMBOL || operator == TOKEN_DIV_SYMBOL)
	{
		return 2;
	}
	else if (operator == TOKEN_ADD_SYMBOL || operator == TOKEN_SUB_SYMBOL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void exprBufferAppend(char value)
{
	if (g_interpreter->expression->index + 1 >= EXPR_BUFFER_SIZE)
	{
		printf("*** ERROR: Expression buffer overflow.\n");
		return;
	}

	g_interpreter->expression->buffer[g_interpreter->expression->index++] = value;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void exprBufferReset()
{
	memset(g_interpreter->expression->buffer, 0, EXPR_BUFFER_SIZE);
	g_interpreter->expression->index = 0;
	g_interpreter->expression->hasError = false;
	ktCharStackClear(g_interpreter->expression->symbolStack);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void exprBufferError()
{
	g_interpreter->expression->hasError = true;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool evaluateExpr(float *out_result)
{
	*out_result = 0.0f;
	FloatStack stack =
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
			if (ktFloatStackIsEmpty(&stack))
				return false;

			float result = 0.0f;

			if (expr[i] == TOKEN_NEG_SYMBOL)
			{
				result = -ktFloatStackPop(&stack);
			}
			else
			{
				float n2 = ktFloatStackPop(&stack);

				if (ktFloatStackIsEmpty(&stack))
					return false;

				float n1 = ktFloatStackPop(&stack);

				if (expr[i] == TOKEN_ADD_SYMBOL)
				{
					result = n1 + n2;
				}
				else if (expr[i] == TOKEN_SUB_SYMBOL)
				{
					result = n1 - n2;
				}
				else if (expr[i] == TOKEN_MUL_SYMBOL)
				{
					result = n1 * n2;
				}
				else if (expr[i] == TOKEN_DIV_SYMBOL)
				{
					// TODO: Check for division by zero.
					result = n1 / n2;
				}
				else if (expr[i] == TOKEN_POW_SYMBOL)
				{
					result = powf(n1, n2);
				}
			}

			ktFloatStackPush(&stack, result);
		}
		else
		{
			size_t index = (size_t)expr[i] - (size_t)'A';
			ktFloatStackPush(&stack, g_interpreter->memory->vars[index]);
		}
	}

	if (!ktFloatStackIsEmpty(&stack))
	{
		*out_result = ktFloatStackPop(&stack);
	}

	return ktFloatStackIsEmpty(&stack);
}

// TODO: *** DEBUG CODE - REMOVE ME FROM FINAL BUILD. ***
void onRpnStmt(int errorCode)
{
	if (errorCode)
		return;

	printf("RPN: %s\n", g_interpreter->expression->buffer);
	printf("symbolStack: %s\n", g_interpreter->expression->symbolStack->data);
}

