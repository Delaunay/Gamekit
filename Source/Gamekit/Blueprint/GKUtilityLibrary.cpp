// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Gamekit/Blueprint/GKUtilityLibrary.h"

#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Math/Rotator.h"
#include "Math/RotationMatrix.h"
#include "Components/PanelSlot.h"
#include "ClearQuad.h"

#include "GKCoordinateLibrary.h"
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


FVector UGKUtilityLibrary::GetFogOfWarMapSize(const UObject *WorldContext) {
    UWorld* World = GEngine->GetWorldFromContextObject(
        WorldContext, 
        EGetWorldErrorMode::LogAndReturnNull
    );

    TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsOfClass(World, AGKFogOfWarVolume::StaticClass(), OutActors);

    if (OutActors.Num() >= 1)
    {
        auto Size = Cast<AGKFogOfWarVolume>(OutActors[0])->GetMapSize();
        return FVector(Size.X, Size.Y, Size.Z);
    }

    return FVector();
}


void UGKUtilityLibrary::GetControllerFieldOfView(const UObject *          World,
                                                 class APlayerController *Controller,
                                                 ETraceTypeQuery          TraceChannel,
                                                 TArray<FVector> &        Corners,
                                                 FVector2D                Margin)
{
    int32 SizeX = 0;
    int32 SizeY = 0;

    Controller->GetViewportSize(SizeX, SizeY);
    auto ViewportSize = FVector2D(SizeX, SizeY) - Margin;

    TArray<AActor *> ActorsToIgnore;
    FHitResult     OutHit;

    static TArray<FVector2D> ViewportCorners = {
        FVector2D(1, 1), 
        FVector2D(0, 1), 
        FVector2D(0, 0), 
        FVector2D(1, 0)
    };

    Corners.Reset(4);

    for (auto &Corner: ViewportCorners)
    {
        FVector WorldDirection;
        FVector WorldLocation;

        auto Screen = ViewportSize * Corner;

         UGameplayStatics::DeprojectScreenToWorld(
            Controller, 
            Screen, 
            WorldLocation, 
            WorldDirection
        );

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
}


void UGKUtilityLibrary::DrawPolygon(const UObject *              WorldContext,
                                    class UCanvasRenderTarget2D *Target,
                                    TArray<FVector>              Corners,
                                    FVector2D                    MapSize,
                                    FLinearColor                 Color,
                                    float                        Thickness
    )
{
    UWorld *World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
    
    UCanvas *                  Canvas;
    FVector2D                  TextureSize;
    FDrawToRenderTargetContext Context;

    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(World, Target, Canvas, TextureSize, Context);
    UKismetRenderingLibrary::ClearRenderTarget2D(World, Target);

    for (int i = 1; i < Corners.Num(); i++)
    {
        Canvas->K2_DrawLine(
            UGKCoordinateLibrary::ToScreenCoordinate(Corners[i - 1], MapSize, TextureSize),
            UGKCoordinateLibrary::ToScreenCoordinate(Corners[i], MapSize, TextureSize),
            Thickness, 
            Color);
    }

    auto Last = Corners.Num() - 1;
    Canvas->K2_DrawLine(
        UGKCoordinateLibrary::ToScreenCoordinate(Corners[Last], MapSize, TextureSize),
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


UWidget *UGKUtilityLibrary::GetPanelSlotContent(UPanelSlot *Slot) {
    return Slot->Content;
}

class UWidget *UGKUtilityLibrary::GetWidgetUnderCursor() {
    // Get a reference to the singleton instance of the slate application.
     FSlateApplication& App = FSlateApplication::Get();
 
     // Find a "widget tree path" of all widgets under the mouse cursor.
     // This path will contain not only the top-level widget, but all widgets underneath.
     // For example, if the mouse cursor was over a Button with a Text widget inside of it, then the last 
     // widget in the widget path would be the Text widget, and the next to last widget would be the Button widget.
     FWidgetPath WidgetsUnderCursor = App.LocateWindowUnderMouse(
         App.GetCursorPos(), 
         App.GetInteractiveTopLevelWindows()
     );
 
     FString Result = TEXT( "" );
     if (WidgetsUnderCursor.IsValid() )
     {
         auto Last = WidgetsUnderCursor.Widgets.Num() - 1;
         FArrangedWidget& Widget = WidgetsUnderCursor.Widgets[Last];
         return nullptr;
     }
    
     return nullptr;
}


EGK_ItemSlot UGKUtilityLibrary::ItemSlotFromInteger(int SlotId) { return EGK_ItemSlot(SlotId); }


float UGKUtilityLibrary::GetYaw(FVector Origin, FVector Target) {
    auto TargetRotator = UKismetMathLibrary::FindLookAtRotation(Origin, Target);
    auto TargetYaw = TargetRotator.Yaw;

    if (FMath::Abs(TargetYaw) > 180)
    {
        TargetYaw = FMath::RadiansToDegrees(FMath::Asin(FMath::Sin(FMath::DegreesToRadians(TargetYaw))));
    }

    return TargetYaw;
}


void UGKUtilityLibrary::GetVisibleBounds(FVector Location, AActor* Actor, FVector& OutMin, FVector& OutMax) {
    FVector Origin;
    FVector BoxExtent;
    Actor->GetActorBounds(true, Origin, BoxExtent);

    BoxExtent.Z = 0;
    Origin.Z = Location.Z;

    TArray<FVector> Corners = {
        Origin + BoxExtent,
        Origin + BoxExtent * FVector(-1, 1, 0),
        Origin - BoxExtent,
        Origin + BoxExtent * FVector(1, -1, 0)
    };

    #define GET_YAW(x, y) UKismetMathLibrary::FindLookAtRotation(x, y).Yaw

    OutMax = Corners[0];
    OutMin = Corners[0];

    auto  Dir      = Location;
    float AngleMax = GET_YAW(Dir, OutMax);
    float AngleMin = GET_YAW(Dir, OutMin);

    // The Corners that matters are the one with the widest angles
    // but this does not really do that it finds the min max angles
    for (int i = 1; i < 4; i++)
    {
        float Angle = GET_YAW(Dir, Corners[i]);

        if (Angle > AngleMax)
        {
            AngleMax = Angle;
            OutMax = Corners[i];
        }

        if (Angle < AngleMin)
        {
            AngleMin = Angle;
            OutMin = Corners[i];
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
        }
    );
}