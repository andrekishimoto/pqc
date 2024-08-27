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
#include <stdlib.h>
#include <string.h>
#include "double_stack.h"
#include "utils.h"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ktDoubleStack* ktDoubleStackCreate()
{
	ktDoubleStack* stack = malloc(sizeof(ktDoubleStack));
	ktDoubleStackClear(stack);

	return stack;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktDoubleStackDestroy(ktDoubleStack* stack)
{
	SAFE_DELETE(stack);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktDoubleStackPush(ktDoubleStack* stack, double value)
{
	if (!stack || ktDoubleStackIsFull(stack))
		return;

	stack->data[stack->count] = value;
	++stack->count;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
double ktDoubleStackPop(ktDoubleStack* stack)
{
	if (!stack || ktDoubleStackIsEmpty(stack))
		return '\0';

	--stack->count;
	double top = stack->data[stack->count];
	stack->data[stack->count] = '\0';

	return top;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
double ktDoubleStackTop(ktDoubleStack* stack)
{
	if (!stack || ktDoubleStackIsEmpty(stack))
		return '\0';

	return stack->data[stack->count - 1];
}
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktDoubleStackClear(ktDoubleStack* stack)
{
	if (stack)
	{
		memset(stack->data, 0, KT_DOUBLE_STACK_SIZE * sizeof(double));
		stack->count = 0;
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool ktDoubleStackIsEmpty(ktDoubleStack* stack)
{
	return stack && stack->count == 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool ktDoubleStackIsFull(ktDoubleStack* stack)
{
	return stack && stack->count == KT_DOUBLE_STACK_SIZE;
}
