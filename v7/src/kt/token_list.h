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

#ifndef __KISHITECH_TOKEN_LIST_H__
#define __KISHITECH_TOKEN_LIST_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stddef.h>
#include <stdbool.h>
#include "token_node.h"

//------------------------------------------------------------------------------
// Custom types, structs, etc.
//------------------------------------------------------------------------------
typedef struct ktTokenList ktTokenList;

struct ktTokenList
{
	ktTokenNode* head;
	ktTokenNode* tail;
	ktTokenNode* cursor;
	size_t count;
};

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
ktTokenList* ktTokenListCreate(void);
void ktTokenListDestroy(ktTokenList* list);
void ktTokenListInsert(ktTokenList* list, ktToken* token);
void ktTokenListAppend(ktTokenList* list, ktToken* token);
void ktTokenListRemoveHead(ktTokenList* list);
void ktTokenListRemoveTail(ktTokenList* list);
void ktTokenListClear(ktTokenList* list);
bool ktTokenListIsEmpty(const ktTokenList* list);
void ktTokenListCursorReset(ktTokenList* list);
void ktTokenListCursorAdvance(ktTokenList* list);
void ktTokenListCursorRetreat(ktTokenList* list);

#endif // __KISHITECH_TOKEN_LIST_H__
