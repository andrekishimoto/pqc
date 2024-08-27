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
// Memory leak detection using MS Visual Studio.
//------------------------------------------------------------------------------
#if defined(_MSC_VER)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "utils.h"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int main()
{
	// Memory leak detection using MS Visual Studio.
#if defined(_MSC_VER)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	puts("[BEGIN]=========================================================================");

	ktInterpreterCreate();
	const char* TOKEN_EXIT = "EXIT";
	const size_t TOKEN_EXIT_LENGTH = strlen(TOKEN_EXIT);
	char str[80] = { 0 };
	size_t strLength = 0;
	size_t length = 0;

	do
	{
		printf("> ");
		fgets(str, 80, stdin);
		strLength = strlen(str);
		if (str[strLength - 1] == '\n')
		{
			str[strLength - 1] = '\0';
		}
		strLength = strlen(str);
		length = ktMax(strLength, TOKEN_EXIT_LENGTH);

		ktInterpreterRun(ktStringToUpper(str));

	} while (strncmp(str, TOKEN_EXIT, length) != 0 || strLength == 0);

	ktInterpreterDestroy();

	puts("===========================================================================[END]");
	return EXIT_SUCCESS;
}
