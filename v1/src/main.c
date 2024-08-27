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

#include "token.h"
#include "token_node.h"
#include "token_list.h"
#include "utils.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN(__call__, __param__)	do { Token* __token__ = __call__(__param__); ktTokenPrint(__token__); ktTokenDestroy(__token__); } while(0)
#define I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN_NO_PARAMS(__call__)	do { Token* __token__ = __call__(); ktTokenPrint(__token__); ktTokenDestroy(__token__); } while(0)

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void printList(const TokenList * list)
{
	puts(">>>>> list->head >>>>>");
	TokenNode* iterator = list->head;
	while (iterator)
	{
		ktTokenPrint(iterator->token);
		iterator = iterator->next;
	}
	puts("<<<<< list->tail >>>>>");
	iterator = list->tail;
	while (iterator)
	{
		ktTokenPrint(iterator->token);
		iterator = iterator->previous;
	}
	puts("<<<<< list->head <<<<<");
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

	puts("[BEGIN]");

	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN(ktTokenCreateString, "Hello, World!");
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN(ktTokenCreateVar, 'a');
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN(ktTokenCreateNumber, 1.2345f);
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN(ktTokenCreateSymbol, TOKEN_EQUALS);
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN(ktTokenCreateSymbol, TOKEN_ADD);
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN(ktTokenCreateSymbol, TOKEN_SUB);
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN(ktTokenCreateSymbol, TOKEN_MUL);
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN(ktTokenCreateSymbol, TOKEN_DIV);
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN(ktTokenCreateSymbol, TOKEN_POW);
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN(ktTokenCreateSymbol, TOKEN_OPEN_PAREN);
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN(ktTokenCreateSymbol, TOKEN_CLOSE_PAREN);
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN_NO_PARAMS(ktTokenCreateStmtLet);
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN_NO_PARAMS(ktTokenCreateStmtReset);
	I_AM_LAZY_CREATE_PRINT_DESTROY_TOKEN_NO_PARAMS(ktTokenCreateStmtVars);

	puts("----------------------------------------");
	TokenList* list = ktTokenListCreate();
	ktTokenListInsert(list, ktTokenCreateString("Hello, World!"));
	ktTokenListAppend(list, ktTokenCreateVar('x'));
	ktTokenListAppend(list, ktTokenCreateNumber(1.2345f));
	ktTokenListAppend(list, ktTokenCreateSymbol(TOKEN_ADD));
	ktTokenListAppend(list, ktTokenCreateStmtVars());
	ktTokenListInsert(list, ktTokenCreateStmtReset());

	printList(list);

	ktTokenListDestroy(list);
	list = NULL;

	puts("----------------------------------------\n[END]");
	return EXIT_SUCCESS;
}
