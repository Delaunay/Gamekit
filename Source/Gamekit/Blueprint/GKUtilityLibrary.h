// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAbilityStatic.h"
#include "Gamekit/GKGamekitSettings.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"

// Generated
#include "GKUtilityLibrary.generated.h"

UENUM(BlueprintType)
enum class EGKRelativePosition : uint8
{
    None  = 0,
    Top   = 1, // 0001
    Bot   = 2, // 0010
    Left  = 4, // 0100
    Right = 8, // 1000

    TopLeft  = Top | Left,  // 0101 i.e  5
    BotLeft  = Bot | Left,  // 0110 i.e  6
    TopRight = Top | Right, // 1001 i.e  9
    BotRight = Bot | Right, // 1010 i.e 10
};

USTRUCT(BlueprintType)
struct FGKNetworkMetrics
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 PacketLoss;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DownKiB;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float UpKiB;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float PingMs;
};

/**
 *
 */
UCLASS()
class GAMEKIT_API UGKUtilityLibrary: public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    public:
    //! This cannot be exposed to blueprint because fo the const
    //! It is fine though we should create accessor lile the GetMapSize
    static class AWorldSettings const *GetWorldSetting(const UObject *World);

    // Returns the project version set in the 'Project Settings' > 'Description' section
    // of the editor
    UFUNCTION(BlueprintPure, Category = "Project")
    static FString GetProjectVersion();

    //! Return the map size stored inside the Fog of War Volume if any
    UFUNCTION(BlueprintPure, Category = "Level|Size", meta = (WorldContext = "World"))
    static FVector GetFogOfWarMapSize(const UObject *World);

    //! Given a player controller it will output the 4 corners seen by the player
    //! projected on the ground
    UFUNCTION(BlueprintCallable, Category = "Controller", meta = (WorldContext = "World"))
    static void GetControllerFieldOfView(const UObject           *World,
                                         class APlayerController *Controller,
                                         ETraceTypeQuery          TraceChannel,
                                         TArray<FVector>         &Corners,
                                         FVector2D                Margin = FVector2D(0, 0));

    //! Draw a polygon given its corners
    //! It will close the polygon at the end
    UFUNCTION(BlueprintCallable, Category = "Controller", meta = (WorldContext = "World"))
    static void DrawPolygon(const UObject               *World,
                            class UCanvasRenderTarget2D *Target,
                            TArray<FVector>              Corners,
                            FVector2D                    MapSize,
                            FLinearColor                 Color,
                            float                        Thickness);

    UFUNCTION(BlueprintPure, Category = "Rotation")
    static FRotator BetterLookAtRotation(FVector ActorLocation,
                                         FVector LookAt,
                                         FVector UpDirection = FVector(0, 0, 1.f));

    UFUNCTION(BlueprintPure, Category = "Widget")
    static class UWidget *GetPanelSlotContent(class UPanelSlot *Slot);

    // UFUNCTION(BlueprintPure, Category = "Widget")
    // Don't use
    static class UWidget *GetWidgetUnderCursor();

    UFUNCTION(BlueprintPure, Category = "ItemSlot")
    static EGK_ItemSlot ItemSlotFromInteger(int SlotId);

    static float GetYaw(FVector Origin, FVector Target);

    //! Returns the relative postition of location given an actor
    UFUNCTION(BlueprintPure, Category = "Bounds")
    static EGKRelativePosition GetRelativePosition(FVector Origin, FVector BoxExtent, FVector Location);

    UFUNCTION(BlueprintPure, Category = "Bounds")
    static EGKRelativePosition GetRelativePositionFromActor(AActor *Actor, FVector Location);

    //! Returns the two points of an actor that are visible from Location
    //! This uses a lookup table to know which points are visisble
    UFUNCTION(BlueprintPure, Category = "Bounds")
    static void GetVisibleBounds(FVector Location, AActor *Actor, FVector &OutMin, FVector &OutMax);

    //! this is not as stable as `GetVisibleBounds`
    UFUNCTION(BlueprintPure, Category = "Bounds")
    static void GetVisibleBounds_Math(FVector Location, AActor *Actor, FVector &OutMin, FVector &OutMax);

    UFUNCTION(BlueprintCallable, Category = "Rendering")
    static void ClearTexture(class UTexture *Texture, FLinearColor ClearColor);

    UFUNCTION(BlueprintPure, Category = "Multiplayer", meta = (WorldContext = "WorldContext"))
    static FName GameInstanceMode(const UObject *WorldContext);

    UFUNCTION(BlueprintPure, Category = "Multiplayer")
    static FString GetNetConfig(const AActor *Actor);

    static FName NetModeToString(ENetMode NetMode);

    static FName NetRoleToString(ENetRole NetRole);

    UFUNCTION(BlueprintCallable, Category = "Multiplayer", meta = (WorldContext = "WorldContext"))
    static FString GetNetworkPrefix(const UObject *WorldContext);

    UFUNCTION(BlueprintCallable, Category = "Multiplayer", meta = (WorldContext = "WorldContext"))
    static FGKNetworkMetrics GetNetworkMetrics(const UObject *WorldContext);

    UFUNCTION(BlueprintCallable, Category = "Multiplayer", meta = (WorldContext = "WorldContextObject"))
    static APlayerController *GetFirstLocalPlayerController(const UObject *WorldContextObject);

    //! Find the rotation which makes the Target face the player controller's camera
    UFUNCTION(BlueprintPure, Category = "Multiplayer", meta = (WorldContext = "WorldContextObject"))
    static FRotator GetLookAtCamera(const UObject *WorldContextObject, int PlayerIndex, FVector Target);
};
