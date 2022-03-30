// Gamekit
#include "Gamekit/Blueprint/GKUtilityLibrary.h"

// Unreal Engine
#include "Misc/AutomationTest.h"



#define GAMEKIT_TEST(Namespace, Section, Function)\
    IMPLEMENT_SIMPLE_AUTOMATION_TEST(\
        F##Section##Function##Test,\
        "Gamekit." #Namespace " " #Function,\
        EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter\
    )\
    bool F##Section##Function##Test::RunTest(const FString &Parameters)


GAMEKIT_TEST("Blueprint.Utility", UGKUtilityLibrary, GetRelativePosition)
{ 
    auto Origin = FVector(0, 0, 0);
    auto Extent = FVector(1, 1, 0);


    TestEqual("Top", 
              UGKUtilityLibrary::GetRelativePosition(Origin, Extent, FVector(2, 0, 0)), 
              EGKRelativePosition::Top);
    TestEqual("Bot", 
              UGKUtilityLibrary::GetRelativePosition(Origin, Extent, FVector(-2, 0, 0)), 
              EGKRelativePosition::Bot);
    TestEqual("Left",
              UGKUtilityLibrary::GetRelativePosition(Origin, Extent, FVector(0, -2, 0)),
              EGKRelativePosition::Left);
    TestEqual("Right",
              UGKUtilityLibrary::GetRelativePosition(Origin, Extent, FVector(0, 2, 0)),
              EGKRelativePosition::Right);
    TestEqual("TopRight",
              UGKUtilityLibrary::GetRelativePosition(Origin, Extent, FVector(2, 2, 0)),
              EGKRelativePosition::TopRight);
    TestEqual("TopLeft",
              UGKUtilityLibrary::GetRelativePosition(Origin, Extent, FVector(2, -2, 0)),
              EGKRelativePosition::TopLeft);
    TestEqual("BotRight",
              UGKUtilityLibrary::GetRelativePosition(Origin, Extent, FVector(-2, -2, 0)),
              EGKRelativePosition::BotRight);
    TestEqual("BotLeft",
              UGKUtilityLibrary::GetRelativePosition(Origin, Extent, FVector(-2, 0, 0)),
              EGKRelativePosition::BotLeft);

    return true;
}
