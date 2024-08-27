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
#include "token_list.h"
#include "utils.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
TokenList* ktTokenListCreate()
{
	TokenList* list = malloc(sizeof(TokenList));
	if (list)
	{
		list->head = NULL;
		list->tail = NULL;
		list->count = 0;
	}

	return list;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListDestroy(TokenList* list)
{
	ktTokenListClear(list);
	SAFE_DELETE(list);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListInsert(TokenList* list, Token* token)
{
	if (!list)
		return;

	TokenNode* node = ktTokenNodeCreate(token);
	if (node)
	{
		if (ktTokenListIsEmpty(list))
		{
			list->tail = node;
		}
		else
		{
			list->head->previous = node;
		}

		node->next = list->head;
		list->head = node;
		++list->count;
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListAppend(TokenList* list, Token* token)
{
	if (!list)
		return;

	TokenNode* node = ktTokenNodeCreate(token);
	if (node)
	{
		if (ktTokenListIsEmpty(list))
		{
			list->head = node;
		}
		else
		{
			list->tail->next = node;
		}

		node->previous = list->tail;
		list->tail = node;
		++list->count;
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListRemoveHead(TokenList* list)
{
	if (!list || ktTokenListIsEmpty(list))
		return;

	TokenNode* toRemove = list->head;

	list->head = list->head->next;
	if (list->head)
	{
		list->head->previous = NULL;
	}
	else
	{
		list->tail = NULL;
	}
	--list->count;

	ktTokenNodeDestroy(toRemove);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListRemoveTail(TokenList* list)
{
	if (!list)
		return;

	if (list->head == list->tail)
	{
		ktTokenListRemoveHead(list);
		return;
	}

	TokenNode* toRemove = list->tail;

	list->tail = list->tail->previous;
	if (list->tail)
	{
		list->tail->next = NULL;
	}
	--list->count;

	ktTokenNodeDestroy(toRemove);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListClear(TokenList* list)
{
	if (!list)
		return;

	while (!ktTokenListIsEmpty(list))
	{
		ktTokenListRemoveHead(list);
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool ktTokenListIsEmpty(const TokenList* list)
{
	return list != NULL && list->head == NULL;
}
