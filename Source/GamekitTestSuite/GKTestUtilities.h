#pragma once

// Unreal Engine
#include "Misc/AutomationTest.h"

#define GAMEKIT_TEST(Namespace, Section, Function)                                  \
    IMPLEMENT_SIMPLE_AUTOMATION_TEST(F##Section##Function##Test,                    \
                                     "Gamekit." #Namespace " " #Function,           \
                                     EAutomationTestFlags::ApplicationContextMask | \
                                             EAutomationTestFlags::EngineFilter)    \
    bool F##Section##Function##Test::RunTest(const FString &Parameters)
