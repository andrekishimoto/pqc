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

#ifndef __KISHITECH_DOUBLE_STACK_H__
#define __KISHITECH_DOUBLE_STACK_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <ctype.h>
#include <stdbool.h>

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
typedef struct ktDoubleStack ktDoubleStack;

enum ktDoubleStackConstants
{
	KT_DOUBLE_STACK_SIZE = 32,
};

struct ktDoubleStack
{
	double data[KT_DOUBLE_STACK_SIZE];
	size_t count;
};

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
ktDoubleStack* ktDoubleStackCreate();
void ktDoubleStackDestroy(ktDoubleStack* stack);
void ktDoubleStackPush(ktDoubleStack* stack, double value);
double ktDoubleStackPop(ktDoubleStack* stack);
double ktDoubleStackTop(ktDoubleStack* stack);
void ktDoubleStackClear(ktDoubleStack* stack);
bool ktDoubleStackIsEmpty(ktDoubleStack* stack);
bool ktDoubleStackIsFull(ktDoubleStack* stack);

#endif // __KISHITECH_DOUBLE_STACK_H__
