#include "Misc/AutomationTest.h"

// namespace EAutomationTestFlags
// {
// enum Type
// {
//     EditorContext          = 0x00000001,
//     ClientContext          = 0x00000002,
//     ServerContext          = 0x00000004,
//     CommandletContext      = 0x00000008,
//     ApplicationContextMask = EditorContext | ClientContext | ServerContext | CommandletContext,
//     NonNullRHI             = 0x00000100,
//     RequiresUser           = 0x00000200,
//     FeatureMask            = NonNullRHI | RequiresUser,
//     Disabled               = 0x00010000,
//     CriticalPriority       = 0x00100000,
//     HighPriority           = 0x00200000,
//     HighPriorityAndAbove   = CriticalPriority | HighPriority,
//     MediumPriority         = 0x00400000,
//     MediumPriorityAndAbove = CriticalPriority | HighPriority | MediumPriority,
//     LowPriority            = 0x00800000,
//     PriorityMask           = CriticalPriority | HighPriority | MediumPriority | LowPriority,
//     SmokeFilter            = 0x01000000,
//     EngineFilter           = 0x02000000,
//     ProductFilter          = 0x04000000,
//     PerfFilter             = 0x08000000,
//     StressFilter           = 0x10000000,
//     NegativeFilter         = 0x20000000,
//     FilterMask             = SmokeFilter | EngineFilter | ProductFilter | PerfFilter | StressFilter | NegativeFilter,
// }
// } // namespace EAutomationTestFlags

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGKBlueprintUtilityTest,
                                  "Gamekit.Blueprint.Utilities",
                                  EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

void FGKBlueprintUtilityTest::GetTests(TArray<FString> &OutBeautifiedNames, TArray<FString> &OutTestCommands) const
{
    OutBeautifiedNames.Add("SubTest1");
    OutTestCommands.Add("1");
}

bool FGKBlueprintUtilityTest::RunTest(const FString &Parameters)
{
    FString Id = Parameters;

    // Id is either "1" or "2".
    // Use it to test something.

    return true; // Or false.
}