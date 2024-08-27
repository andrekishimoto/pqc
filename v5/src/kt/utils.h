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

#ifndef __KISHITECH_UTILS_H__
#define __KISHITECH_UTILS_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stdbool.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define SAFE_DELETE(ptr)	do { if (ptr) { free(ptr); ptr = NULL; } } while(0)

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
bool ktStringCopy(char** destination, const char* source);
bool ktStringCopyInterval(char** destination, const char* source, size_t startIndex, size_t endIndex);
char* ktStringToUpper(char* string);
void ktStringDestroy(char** string);

size_t ktMax(size_t a, size_t b);
size_t ktMin(size_t a, size_t b);

#endif // __KISHITECH_UTILS_H__
