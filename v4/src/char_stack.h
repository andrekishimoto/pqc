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

#ifndef __KISHITECH_CHAR_STACK_H__
#define __KISHITECH_CHAR_STACK_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <ctype.h>
#include <stdbool.h>

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
typedef struct CharStack CharStack;

enum CharStackConstants
{
	CHAR_STACK_SIZE = 32,
};

struct CharStack
{
	char data[CHAR_STACK_SIZE];
	size_t count;
};

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
CharStack* ktCharStackCreate();
void ktCharStackDestroy(CharStack* stack);
void ktCharStackPush(CharStack* stack, char value);
char ktCharStackPop(CharStack* stack);
char ktCharStackTop(CharStack* stack);
void ktCharStackClear(CharStack* stack);
bool ktCharStackIsEmpty(CharStack* stack);
bool ktCharStackIsFull(CharStack* stack);

#endif // __KISHITECH_CHAR_STACK_H__
