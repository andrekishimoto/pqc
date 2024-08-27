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

#ifndef __KISHITECH_FLOAT_STACK_H__
#define __KISHITECH_FLOAT_STACK_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <ctype.h>
#include <stdbool.h>

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
typedef struct ktFloatStack ktFloatStack;

enum ktFloatStackConstants
{
	KT_FLOAT_STACK_SIZE = 32,
};

struct ktFloatStack
{
	float data[KT_FLOAT_STACK_SIZE];
	size_t count;
};

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
ktFloatStack* ktFloatStackCreate();
void ktFloatStackDestroy(ktFloatStack* stack);
void ktFloatStackPush(ktFloatStack* stack, float value);
float ktFloatStackPop(ktFloatStack* stack);
float ktFloatStackTop(ktFloatStack* stack);
void ktFloatStackClear(ktFloatStack* stack);
bool ktFloatStackIsEmpty(ktFloatStack* stack);
bool ktFloatStackIsFull(ktFloatStack* stack);

#endif // __KISHITECH_FLOAT_STACK_H__
