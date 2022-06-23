// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Blueprint/GKUtilityLibrary.h"

// Gamekit
#include "Gamekit/Blueprint/GKCoordinateLibrary.h"
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/GKLog.h"
#include "Gamekit/GKWorldSettings.h"

// Unreal Engine
#include "ClearQuad.h"
#include "Components/PanelSlot.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/NetConnection.h"
#include "Engine/NetDriver.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/RotationMatrix.h"
#include "Math/Rotator.h"

AWorldSettings const *UGKUtilityLibrary::GetWorldSetting(const UObject *WorldContext)
{
    UWorld *World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);

    if (!World)
    {
        return nullptr;
    }

    return World->GetWorldSettings();
}

FString UGKUtilityLibrary::GetProjectVersion()
{
    FString ProjectVersion;

    GConfig->GetString(
            TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectVersion"), ProjectVersion, GGameIni);

    return ProjectVersion;
}

FVector UGKUtilityLibrary::GetFogOfWarMapSize(const UObject *WorldContext)
{
    UWorld *World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);

    TArray<AActor *> OutActors;
    UGameplayStatics::GetAllActorsOfClass(World, AGKFogOfWarVolume::StaticClass(), OutActors);

    if (OutActors.Num() >= 1)
    {
        auto Size = Cast<AGKFogOfWarVolume>(OutActors[0])->GetMapSize();
        return FVector(Size.X, Size.Y, Size.Z);
    }

    return FVector();
}

void UGKUtilityLibrary::GetControllerFieldOfView(const UObject           *World,
                                                 class APlayerController *Controller,
                                                 ETraceTypeQuery          TraceChannel,
                                                 TArray<FVector>         &Corners,
                                                 FVector2D                Margin)
{
    int32 SizeX = 0;
    int32 SizeY = 0;

    Controller->GetViewportSize(SizeX, SizeY);
    auto ViewportSize = FVector2D(SizeX, SizeY) - Margin;

    TArray<AActor *> ActorsToIgnore;
    FHitResult       OutHit;

    static TArray<FVector2D> ViewportCorners = {FVector2D(0, 0), FVector2D(1, 0), FVector2D(1, 1), FVector2D(0, 1)};

    Corners.Reset(4);

    for (auto &Corner: ViewportCorners)
    {
        FVector WorldDirection;
        FVector WorldLocation;

        auto Screen = ViewportSize * Corner;

        UGameplayStatics::DeprojectScreenToWorld(Controller, Screen, WorldLocation, WorldDirection);

        UKismetSystemLibrary::LineTraceSingle(World,
                                              WorldLocation,
                                              WorldLocation + WorldDirection * 5000.f,
                                              TraceChannel,
                                              false,
                                              ActorsToIgnore,
                                              EDrawDebugTrace::None,
                                              OutHit,
                                              true);

        Corners.Add(OutHit.Location);
    }

    // Makes sure the lines are perfectly horizontal
    // without it they is a small offset
    Corners[0].X = Corners[1].X;
    Corners[2].X = Corners[3].X;
}

void UGKUtilityLibrary::DrawPolygon(const UObject               *WorldContext,
                                    class UCanvasRenderTarget2D *Target,
                                    TArray<FVector>              Corners,
                                    FVector2D                    MapSize,
                                    FLinearColor                 Color,
                                    float                        Thickness)
{
    UWorld *World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);

    UCanvas                   *Canvas;
    FVector2D                  TextureSize;
    FDrawToRenderTargetContext Context;

    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(World, Target, Canvas, TextureSize, Context);
    UKismetRenderingLibrary::ClearRenderTarget2D(World, Target);

    for (int i = 1; i < Corners.Num(); i++)
    {
        Canvas->K2_DrawLine(UGKCoordinateLibrary::ToScreenCoordinate(Corners[i - 1], MapSize, TextureSize),
                            UGKCoordinateLibrary::ToScreenCoordinate(Corners[i], MapSize, TextureSize),
                            Thickness,
                            Color);
    }

    auto Last = Corners.Num() - 1;
    Canvas->K2_DrawLine(UGKCoordinateLibrary::ToScreenCoordinate(Corners[Last], MapSize, TextureSize),
                        UGKCoordinateLibrary::ToScreenCoordinate(Corners[0], MapSize, TextureSize),
                        Thickness,
                        Color);

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(World, Context);
}

FRotator UGKUtilityLibrary::BetterLookAtRotation(FVector ActorLocation, FVector LookAt, FVector UpDirection)
{
    ActorLocation.Z = 0;
    LookAt.Z        = 0;

    float Yaw = FMath::Acos(FVector::DotProduct(ActorLocation, LookAt) / (ActorLocation.Size2D() * LookAt.Size2D()));
    return FRotator(0, Yaw, 0);
}

UWidget *UGKUtilityLibrary::GetPanelSlotContent(UPanelSlot *Slot) { return Slot->Content; }

class UWidget *UGKUtilityLibrary::GetWidgetUnderCursor()
{
    // Get a reference to the singleton instance of the slate application.
    FSlateApplication &App = FSlateApplication::Get();

    // Find a "widget tree path" of all widgets under the mouse cursor.
    // This path will contain not only the top-level widget, but all widgets underneath.
    // For example, if the mouse cursor was over a Button with a Text widget inside of it, then the last
    // widget in the widget path would be the Text widget, and the next to last widget would be the Button widget.
    FWidgetPath WidgetsUnderCursor =
            App.LocateWindowUnderMouse(App.GetCursorPos(), App.GetInteractiveTopLevelWindows());

    FString Result = TEXT("");
    if (WidgetsUnderCursor.IsValid())
    {
        auto             Last   = WidgetsUnderCursor.Widgets.Num() - 1;
        FArrangedWidget &Widget = WidgetsUnderCursor.Widgets[Last];
        return nullptr;
    }

    return nullptr;
}

EGK_ItemSlot UGKUtilityLibrary::ItemSlotFromInteger(int SlotId) { return EGK_ItemSlot(SlotId); }

float UGKUtilityLibrary::GetYaw(FVector Origin, FVector Target)
{
    auto TargetRotator = UKismetMathLibrary::FindLookAtRotation(Origin, Target);
    auto TargetYaw     = TargetRotator.Yaw;

    if (FMath::Abs(TargetYaw) > 180)
    {
        TargetYaw = FMath::RadiansToDegrees(FMath::Asin(FMath::Sin(FMath::DegreesToRadians(TargetYaw))));
    }

    return TargetYaw;
}

EGKRelativePosition UGKUtilityLibrary::GetRelativePosition(FVector Origin, FVector BoxExtent, FVector Location)
{
    auto  Centered = Location - Origin;
    uint8 Pos      = 0;

    if (Centered.Y < -BoxExtent.Y)
    {
        Pos |= uint8(EGKRelativePosition::Left);
    }
    if (Centered.Y > BoxExtent.Y)
    {
        Pos |= uint8(EGKRelativePosition::Right);
    }
    if (Centered.X < -BoxExtent.X)
    {
        Pos |= uint8(EGKRelativePosition::Bot);
    }
    if (Centered.X > BoxExtent.X)
    {
        Pos |= uint8(EGKRelativePosition::Top);
    }

    return EGKRelativePosition(Pos);
}

EGKRelativePosition UGKUtilityLibrary::GetRelativePositionFromActor(AActor *Actor, FVector Location)
{
    FVector Origin;
    FVector BoxExtent;
    Actor->GetActorBounds(true, Origin, BoxExtent);

    auto  Centered = Location - Origin;
    uint8 Pos      = 0;

    if (Centered.Y < -BoxExtent.Y)
    {
        Pos |= uint8(EGKRelativePosition::Left);
    }
    if (Centered.Y > BoxExtent.Y)
    {
        Pos |= uint8(EGKRelativePosition::Right);
    }
    if (Centered.X < -BoxExtent.X)
    {
        Pos |= uint8(EGKRelativePosition::Bot);
    }
    if (Centered.X > BoxExtent.X)
    {
        Pos |= uint8(EGKRelativePosition::Top);
    }

    return EGKRelativePosition(Pos);
}

// Test This

//                  ( 0,  0, 0) Cube
//  Player Top      ( 1,  0, 0)     # Angle (Top Right, Top, Left)
//  Player Bot      (-1,  0, 0)     # Angle (Bot Right, Bot Left)
//  Player Left     ( 0, -1, 0)     # Angle (Top Left , Bot Left)
//  Player Right    ( 0,  1, 0)     # Angle (Top RIght, Bot Right)
//
//  Player TopRight ( 1,  1, 0)     # Angle (TopLeft,  BotRight)
//  Player TopLeft  ( 1, -1, 0)     # Angle (TopRight, BotLeft)
//  Player BotLeft  (-1, -1, 0)     # Angle (TopLeft, BotRight)
//  Player BotRIght (-1,  1, 0)     # Angle (TopRight, BotLeft)
//

void UGKUtilityLibrary::GetVisibleBounds(FVector Location, AActor *Actor, FVector &OutMin, FVector &OutMax)
{

    auto Relative = GetRelativePositionFromActor(Actor, Location);

    FVector Origin;
    FVector BoxExtent;
    Actor->GetActorBounds(true, Origin, BoxExtent);

    auto TopRight = Origin + BoxExtent;
    auto BotRight = Origin + BoxExtent * FVector(-1, 1, 0);
    auto BotLeft  = Origin - BoxExtent;
    auto TopLeft  = Origin + BoxExtent * FVector(1, -1, 0);

    /*
    UE_LOG(LogGamekit,
           Warning,
           TEXT("Position %d"), int(Relative));
    */

    // clang-format off
    switch (Relative)
    {
        case EGKRelativePosition::Top        : {OutMin = TopLeft;  OutMax = TopRight; return; }
        case EGKRelativePosition::Bot        : {OutMin = BotLeft;  OutMax = BotRight; return; }
        case EGKRelativePosition::Left       : {OutMin = TopLeft;  OutMax = BotLeft;  return; }
        case EGKRelativePosition::Right      : {OutMin = BotRight; OutMax = TopRight; return; }
        case EGKRelativePosition::TopRight   : {OutMin = TopLeft;  OutMax = BotRight; return; }
        case EGKRelativePosition::TopLeft    : {OutMin = TopRight; OutMax = BotLeft;  return; }
        case EGKRelativePosition::BotRight   : {OutMin = TopRight; OutMax = BotLeft;  return; }
        case EGKRelativePosition::BotLeft    : {OutMin = TopLeft;  OutMax = BotRight; return; }

        // This means the location is inside the bounding box
        case EGKRelativePosition::None       : { return;}
    }
    // clang-format on

    UE_LOG(LogGamekit, Warning, TEXT("Point is inside the actors' bounding box"));
}

void UGKUtilityLibrary::GetVisibleBounds_Math(FVector Location, AActor *Actor, FVector &OutMin, FVector &OutMax)
{
    FVector Origin;
    FVector BoxExtent;
    Actor->GetActorBounds(true, Origin, BoxExtent);

    BoxExtent.Z = 0;
    Origin.Z    = Location.Z;

    TArray<FVector> Corners = {Origin + BoxExtent,                      // Top Right
                               Origin + BoxExtent * FVector(-1, 1, 0),  // Bot Right
                               Origin - BoxExtent,                      // Bot Left
                               Origin + BoxExtent * FVector(1, -1, 0)}; // Top Left

    auto  Dir      = Location;
    float MidAngle = GetYaw(Dir, Origin);

    auto GetYaw = [&MidAngle](FVector x, FVector y) -> float
    {
        auto yaw = UKismetMathLibrary::FindLookAtRotation(x, y).Yaw;

        return FMath::Abs(MidAngle) * FMath::Sign(yaw) - yaw;
    };

    OutMax = Corners[0];
    OutMin = Corners[0];

    float AngleMax = GetYaw(Dir, OutMax);
    float AngleMin = GetYaw(Dir, OutMin);

    // The Corners that matters are the one with the widest angles
    // but this does not really do that it finds the min max angles
    for (int i = 1; i < 4; i++)
    {
        float Angle = GetYaw(Dir, Corners[i]);

        if (Angle > AngleMax)
        {
            AngleMax = Angle;
            OutMax   = Corners[i];
        }

        if (Angle < AngleMin)
        {
            AngleMin = Angle;
            OutMin   = Corners[i];
        }
    }
}

void UGKUtilityLibrary::ClearTexture(class UTexture *Texture, FLinearColor ClearColor)
{
    ENQUEUE_RENDER_COMMAND(ClearRTCommand)
    (
            [Texture, ClearColor](FRHICommandList &RHICmdList)
            {
                FRHIRenderPassInfo RPInfo(Texture->GetResource()->GetTexture2DRHI(),
                                          ERenderTargetActions::DontLoad_Store);

                TransitionRenderPassTargets(RHICmdList, RPInfo);
                RHICmdList.BeginRenderPass(RPInfo, TEXT("ClearTexture"));
                DrawClearQuad(RHICmdList, ClearColor);
                RHICmdList.EndRenderPass();

                RHICmdList.Transition(FRHITransitionInfo(
                        Texture->GetResource()->GetTexture2DRHI(), ERHIAccess::RTV, ERHIAccess::SRVMask));
            });
}

FName UGKUtilityLibrary::GameInstanceMode(const UObject *WorldContext)
{
    static FName Server = "Server";
    static FName Client = "Client";

    UWorld *World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);

    switch (World->GetGameInstance()->IsDedicatedServerInstance())
    {
    case true:
        return Server;
    case false:
        return Client;
    }

    return NAME_None;
}

FName UGKUtilityLibrary::NetModeToString(ENetMode NetMode)
{
    switch (NetMode)
    {
    case NM_Standalone:
        return "Standalone";
    case NM_DedicatedServer:
        return "Dedicated";
    case NM_ListenServer:
        return "Listen";
    case NM_Client:
        return "Client";
    }
    return NAME_None;
}

FName UGKUtilityLibrary::NetRoleToString(ENetRole NetRole)
{
    switch (NetRole)
    {
    case ROLE_SimulatedProxy:
        return "Simulated";
    case ROLE_AutonomousProxy:
        return "AutoProxy";
    case ROLE_Authority:
        return "Authority";
    }
    return NAME_None;
}

FString UGKUtilityLibrary::GetNetConfig(const AActor *Actor)
{
    TArray<FStringFormatArg> Frags = {FStringFormatArg(NetModeToString(Actor->GetNetMode()).ToString()),
                                      FStringFormatArg(NetRoleToString(Actor->GetLocalRole()).ToString()),
                                      FStringFormatArg(NetRoleToString(Actor->GetRemoteRole()).ToString())};
    return FString::Format(TEXT("[NM: {0}] [LR: {1}] [RR: {2}]"), Frags);
}

FGKNetworkMetrics UGKUtilityLibrary::GetNetworkMetrics(const UObject *WorldContext)
{
    UWorld *World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);

    if (!World)
    {
        return FGKNetworkMetrics();
    }

    auto Driver = World->GetNetDriver();

    if (!Driver)
    {
        return FGKNetworkMetrics();
    }

    auto Metrics       = FGKNetworkMetrics();
    Metrics.PingMs     = -1;
    Metrics.PacketLoss = Driver->InPacketsLost + Driver->OutPacketsLost;
    Metrics.DownKiB    = float(Driver->InBytesPerSecond) / 1024.f;
    Metrics.UpKiB      = float(Driver->OutBytesPerSecond) / 1024.f;

    if (Driver->ServerConnection && Driver->ServerConnection->PlayerController &&
        Driver->ServerConnection->PlayerController->PlayerState)
    {
        Metrics.PingMs = Driver->ServerConnection->PlayerController->PlayerState->ExactPing;
    }

    return Metrics;
    // Unresolved symbol ??
    // the driver should be inside the Engine module though
    // Driver->DrawNetDriverDebug();

    // Connection to the server
    // auto Connection = Driver->ServerConnection;
}

APlayerController *UGKUtilityLibrary::GetFirstLocalPlayerController(const UObject *WorldContextObject)
{
    UWorld *World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);

    for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController *PlayerController = Iterator->Get();
        if (PlayerController)
        {
            ULocalPlayer *LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player);

            if (LocalPlayer)
            {
                return PlayerController;
            }
        }
    }

    return nullptr;
}

FString UGKUtilityLibrary::GetNetworkPrefix(const UObject *WorldContextObject)
{
    FString Prefix;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) // Do not Print in Shipping or Test
    UWorld *World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
    if (World)
    {
        if (World->WorldType == EWorldType::PIE)
        {
            switch (World->GetNetMode())
            {
            case NM_Client:
                // GPlayInEditorID 0 is always the server, so 1 will be first client.
                // You want to keep this logic in sync with GeneratePIEViewportWindowTitle and
                // UpdatePlayInEditorWorldDebugString
                Prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID);
                break;
            case NM_DedicatedServer:
            case NM_ListenServer:
                Prefix = FString::Printf(TEXT("Server: "));
                break;
            case NM_Standalone:
                break;
            }
        }
    }
#endif

    return Prefix;
}

FRotator UGKUtilityLibrary::GetLookAtCamera(const UObject *WorldContextObject, int PlayerIndex, FVector Target)
{
    UWorld *World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);

    APlayerController *PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex);

    FVector  Location = PlayerController->PlayerCameraManager->GetCameraLocation();
    FRotator Rot      = PlayerController->PlayerCameraManager->GetCameraRotation();

    FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Target, Location);

    return  FRotator(-Rot.Pitch, LookAtRot.Yaw, LookAtRot.Roll);
}