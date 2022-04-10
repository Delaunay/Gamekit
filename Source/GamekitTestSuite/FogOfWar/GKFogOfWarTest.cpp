// Gamekit
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"

// Gamekit Tests
#include "GamekitTestSuite/GKTestUtilities.h"

// Unreal Engine Tests
#include "Tests/AutomationEditorCommon.h"


GAMEKIT_TEST(FogOfWar.Something, AGKFogOfWarVolume, GetRelativePosition)
{
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();

    return true;
}
