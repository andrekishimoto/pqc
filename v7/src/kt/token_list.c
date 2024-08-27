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
ktTokenList* ktTokenListCreate(void)
{
	ktTokenList* list = malloc(sizeof(ktTokenList));
	if (list)
	{
		list->head = NULL;
		list->tail = NULL;
		list->cursor = NULL;
		list->count = 0;
	}

	return list;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListDestroy(ktTokenList* list)
{
	ktTokenListClear(list);
	SAFE_DELETE(list);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListInsert(ktTokenList* list, ktToken* token)
{
	if (!list)
		return;

	ktTokenNode* node = ktTokenNodeCreate(token);
	if (node)
	{
		if (ktTokenListIsEmpty(list))
		{
			list->tail = node;
			ktTokenListCursorReset(list);
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
void ktTokenListAppend(ktTokenList* list, ktToken* token)
{
	if (!list)
		return;

	ktTokenNode* node = ktTokenNodeCreate(token);
	if (node)
	{
		if (ktTokenListIsEmpty(list))
		{
			list->head = node;
			ktTokenListCursorReset(list);
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
void ktTokenListRemoveHead(ktTokenList* list)
{
	if (!list || ktTokenListIsEmpty(list))
		return;

	ktTokenNode* toRemove = list->head;

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

	if (list->cursor == toRemove)
	{
		list->cursor = list->head;
	}

	ktTokenNodeDestroy(toRemove);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListRemoveTail(ktTokenList* list)
{
	if (!list)
		return;

	if (list->head == list->tail)
	{
		ktTokenListRemoveHead(list);
		return;
	}

	ktTokenNode* toRemove = list->tail;

	list->tail = list->tail->previous;
	if (list->tail)
	{
		list->tail->next = NULL;
	}
	--list->count;

	if (list->cursor == toRemove)
	{
		list->cursor = list->tail;
	}

	ktTokenNodeDestroy(toRemove);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListClear(ktTokenList* list)
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
bool ktTokenListIsEmpty(const ktTokenList* list)
{
	return list != NULL && list->head == NULL;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListCursorReset(ktTokenList* list)
{
	if (!list)
		return;

	list->cursor = list->head;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListCursorAdvance(ktTokenList* list)
{
	if (!list)
		return;

	if (list->cursor)
	{
		list->cursor = list->cursor->next;
		if (!list->cursor)
		{
			list->cursor = list->head;
		}
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ktTokenListCursorRetreat(ktTokenList* list)
{
	if (!list)
		return;

	if (list->cursor)
	{
		list->cursor = list->cursor->previous;
		if (!list->cursor)
		{
			list->cursor = list->tail;
		}
	}
}
