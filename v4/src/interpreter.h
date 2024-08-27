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

#ifndef __KISHITECH_INTERPRETER_H__
#define __KISHITECH_INTERPRETER_H__

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
void ktInterpreterCreate();
void ktInterpreterDestroy();
void ktInterpreterRun(const char* contents);

#endif // __KISHITECH_INTERPRETER_H__
