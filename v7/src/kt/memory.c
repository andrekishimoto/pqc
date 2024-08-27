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
#include <stddef.h>
#include <stdlib.h>
#include "memory.h"
#include "utils.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
ktMemory* ktMemoryCreate(void)
{
    ktMemory* memory = malloc(sizeof(ktMemory));
    if (memory)
    {
        ktMemoryReset(memory);
    }

    return memory;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktMemoryDestroy(ktMemory* memory)
{
    SAFE_DELETE(memory);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktMemoryReset(ktMemory* memory)
{
    if (!memory)
        return;

    for (size_t i = 0; i < KT_VAR_COUNT; ++i)
    {
        memory->hasValue[i] = false;
        memory->vars[i] = 0.0;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktMemorySet(ktMemory* memory, size_t index, double value)
{
    if (!memory || index >= KT_VAR_COUNT)
        return;

    memory->vars[index] = value;
    memory->hasValue[index] = true;
}
