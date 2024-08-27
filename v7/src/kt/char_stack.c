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
#include "char_stack.h"
#include "utils.h"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ktCharStack* ktCharStackCreate(void)
{
	ktCharStack* stack = malloc(sizeof(ktCharStack));
	ktCharStackClear(stack);

	return stack;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktCharStackDestroy(ktCharStack* stack)
{
	SAFE_DELETE(stack);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktCharStackPush(ktCharStack* stack, char value)
{
	if (!stack || ktCharStackIsFull(stack))
		return;

	stack->data[stack->count] = value;
	++stack->count;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
char ktCharStackPop(ktCharStack* stack)
{
	if (!stack || ktCharStackIsEmpty(stack))
		return '\0';

	--stack->count;
	char top = stack->data[stack->count];
	stack->data[stack->count] = '\0';

	return top;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
char ktCharStackTop(ktCharStack* stack)
{
	if (!stack || ktCharStackIsEmpty(stack))
		return '\0';

	return stack->data[stack->count - 1];
}
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktCharStackClear(ktCharStack* stack)
{
	if (stack)
	{
		memset(stack->data, 0, KT_CHAR_STACK_SIZE);
		stack->count = 0;
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool ktCharStackIsEmpty(ktCharStack* stack)
{
	return stack && stack->count == 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool ktCharStackIsFull(ktCharStack* stack)
{
	return stack && stack->count == KT_CHAR_STACK_SIZE;
}
