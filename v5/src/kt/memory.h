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

#ifndef __KISHITECH_MEMORY_H__
#define __KISHITECH_MEMORY_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stdbool.h>

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
enum ktMemoryConstants
{
	KT_VAR_COUNT = 26,
};

typedef struct ktMemory ktMemory;

struct ktMemory
{
	bool hasValue[KT_VAR_COUNT];
	float vars[KT_VAR_COUNT];
};

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
ktMemory* ktMemoryCreate();
void ktMemoryDestroy(ktMemory* memory);
void ktMemoryReset(ktMemory* memory);
void ktMemorySet(ktMemory* memory, size_t index, float value);

#endif // __KISHITECH_MEMORY_H__
