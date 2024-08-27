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
#include "float_stack.h"
#include "utils.h"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
FloatStack* ktFloatStackCreate()
{
	FloatStack* stack = malloc(sizeof(FloatStack));
	ktFloatStackClear(stack);

	return stack;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktFloatStackDestroy(FloatStack* stack)
{
	SAFE_DELETE(stack);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktFloatStackPush(FloatStack* stack, float value)
{
	if (!stack || ktFloatStackIsFull(stack))
		return;

	stack->data[stack->count] = value;
	++stack->count;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
float ktFloatStackPop(FloatStack* stack)
{
	if (!stack || ktFloatStackIsEmpty(stack))
		return '\0';

	--stack->count;
	float top = stack->data[stack->count];
	stack->data[stack->count] = '\0';

	return top;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
float ktFloatStackTop(FloatStack* stack)
{
	if (!stack || ktFloatStackIsEmpty(stack))
		return '\0';

	return stack->data[stack->count - 1];
}
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktFloatStackClear(FloatStack* stack)
{
	if (stack)
	{
		memset(stack->data, 0, FLOAT_STACK_SIZE * sizeof(float));
		stack->count = 0;
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool ktFloatStackIsEmpty(FloatStack* stack)
{
	return stack && stack->count == 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool ktFloatStackIsFull(FloatStack* stack)
{
	return stack && stack->count == FLOAT_STACK_SIZE;
}
