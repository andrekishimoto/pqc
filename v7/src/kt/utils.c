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
// MS Visual Studio-specific (disable C4996 'strncpy unsafe').
//------------------------------------------------------------------------------
#if defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool ktStringCopy(char** destination, const char* source)
{
	size_t length = strlen(source);
	*destination = malloc((length + 1) * sizeof(char));
	if (*destination)
	{
		strncpy(*destination, source, length);
		(*destination)[length] = '\0';

		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool ktStringCopyInterval(char** destination, const char* source, size_t startIndex, size_t endIndex)
{
	size_t length = endIndex - startIndex + 1;
	*destination = malloc((length + 1) * sizeof(char));
	if (*destination)
	{
		strncpy(*destination, &source[startIndex], length);
		(*destination)[length] = '\0';

		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
char* ktStringToUpper(char* string)
{
	char* original = string;

	while (*string)
	{
		*string = toupper(*string);
		++string;
	}

	return original;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ktStringDestroy(char* string)
{
	SAFE_DELETE(string);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
size_t ktMax(size_t a, size_t b)
{
	return a > b ? a : b;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
size_t ktMin(size_t a, size_t b)
{
	return a < b ? a : b;
}
