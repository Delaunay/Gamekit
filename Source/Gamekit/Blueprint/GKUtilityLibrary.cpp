// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/Blueprint/GKUtilityLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "GKWorldSettings.h"

AWorldSettings const*UGKUtilityLibrary::GetWorldSetting(const UObject *WorldContext) {
    UWorld* World = GEngine->GetWorldFromContextObject(
        WorldContext, 
        EGetWorldErrorMode::LogAndReturnNull
    );

    if (!World)
    {
        return nullptr;
    }

    return World->GetWorldSettings();
}

FVector2D UGKUtilityLibrary::GetWorldMapSize(const UObject *World) {
    auto Settings = Cast<AGKWorldSettings const>(GetWorldSetting(World));

    if (!Settings)
        return FVector2D();
        
    return Settings->MapSize;
}

FString UGKUtilityLibrary::GetProjectVersion()
{
    FString ProjectVersion;

    GConfig->GetString(
            TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectVersion"), ProjectVersion, GGameIni);

    return ProjectVersion;
}


FVector2D UGKUtilityLibrary::GetFogOfWarMapSize(const UObject *WorldContext) {
    UWorld* World = GEngine->GetWorldFromContextObject(
        WorldContext, 
        EGetWorldErrorMode::LogAndReturnNull
    );

    TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsOfClass(World, AGKFogOfWarVolume::StaticClass(), OutActors);

    if (OutActors.Num() >= 1)
    {
        auto Size = Cast<AGKFogOfWarVolume>(OutActors[0])->GetMapSize();
        return FVector2D(Size.R, Size.G);
    }

    return FVector2D();
}