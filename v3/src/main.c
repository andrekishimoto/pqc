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

#include "parser.h"
#include "token.h"
#include "token_node.h"
#include "token_list.h"
#include "tokenizer.h"
#include "utils.h"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void printList(const TokenList * list)
{
	TokenNode* iterator = list->head;
	while (iterator)
	{
		ktTokenPrint(iterator->token);
		iterator = iterator->next;
	}
}

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

	Parser* parser = ktParserCreate();
	char str[80] = { 0 };
	size_t length = 0;

	do
	{
		printf("> ");
		fgets(str, 80, stdin);
		length = strlen(str);
		if (str[length - 1] == '\n')
			str[length - 1] = '\0';
		length = strlen(str);
		//printf("(%llu chars) %s\n", length, str);

		ktParserRun(parser, ktStringToUpper(str));
		//printList(parser->tokenList);
	} while (strncmp(str, "EXIT", length) != 0 || length == 0);

	ktParserDestroy(parser);

	puts("===========================================================================[END]");
	return EXIT_SUCCESS;
}
