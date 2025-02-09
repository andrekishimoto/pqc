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
#include <stdlib.h>
#include "kt/interpreter.h"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int main()
{
	// Memory leak detection using MS Visual Studio.
#if defined(_MSC_VER)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	ktInterpreterRun();

	return EXIT_SUCCESS;
}
