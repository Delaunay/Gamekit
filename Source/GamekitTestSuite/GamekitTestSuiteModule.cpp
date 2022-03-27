// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "GamekitTestSuiteModule.h"

#define LOCTEXT_NAMESPACE "GamekitTestSuiteModule"

class FGamekitTestSuiteModule: public IGamekitTestSuiteModule
{
};

IMPLEMENT_MODULE(FGamekitTestSuiteModule, GamekitTestSuiteModule)

#undef LOCTEXT_NAMESPACE
