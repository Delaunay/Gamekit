// Gamekit
#include "Gamekit/Blueprint/GKUtilityLibrary.h"

// Gamekit Test
#include "GamekitTestSuite/GKTestUtilities.h"


GAMEKIT_TEST(Blueprint.Utility, UGKUtilityLibrary, GetRelativePosition)
{
    auto Origin = FVector(0, 0, 0);
    auto Extent = FVector(1, 1, 0);

    auto GetPosition = [&](float x, float y)
    {
        //
        return UGKUtilityLibrary::GetRelativePosition(Origin, Extent, FVector(x, y, 0));
    };

#define CASES(CASE)       \
    CASE(Top, 2, 0)       \
    CASE(Bot, -2, 0)      \
    CASE(Left, 0, -2)     \
    CASE(Right, 0, 2)     \
    CASE(TopRight, 2, 2)  \
    CASE(TopLeft, 2, -2)  \
    CASE(BotRight, -2, 2) \
    CASE(BotLeft, -2, -2) \
    CASE(None, 0, 0)

#define CHECK_CASE(Name, X, Y) TestEqual(#Name, GetPosition(X, Y), EGKRelativePosition::Name);

    CASES(CHECK_CASE)

#undef CASES
#undef CHECK_CASE

    return true;
}
