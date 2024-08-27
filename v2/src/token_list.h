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
typedef struct TokenList TokenList;

struct TokenList
{
	TokenNode* head;
	TokenNode* tail;
	TokenNode* cursor;
	size_t count;
};

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
TokenList* ktTokenListCreate();
void ktTokenListDestroy(TokenList* list);
void ktTokenListInsert(TokenList* list, Token* token);
void ktTokenListAppend(TokenList* list, Token* token);
void ktTokenListRemoveHead(TokenList* list);
void ktTokenListRemoveTail(TokenList* list);
void ktTokenListClear(TokenList* list);
bool ktTokenListIsEmpty(const TokenList* list);
void ktTokenListCursorReset(TokenList* list);
void ktTokenListCursorAdvance(TokenList* list);
void ktTokenListCursorRetreat(TokenList* list);

#endif // __KISHITECH_TOKEN_LIST_H__
