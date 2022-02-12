// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#include "FogOfWar/GKFogOfWarVolume.h"

#include "FogOfWar/GKFogOfWarComponent.h"
#include "FogOfWar/GKFogOfWarLibrary.h"

#include "Blueprint/GKCoordinateLibrary.h"
#include "Blueprint/GKUtilityLibrary.h"

#include "Engine/CollisionProfile.h"
#include "Components/BrushComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/TextureRenderTarget2DArray.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollection.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"

AGKFogOfWarVolume::AGKFogOfWarVolume()
{
    PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> FoWParameterCollection(
            TEXT("MaterialParameterCollection'/Gamekit/FogOfWar/FoWParameters.FoWParameters'"));

    if (FoWParameterCollection.Succeeded())
    {
        UE_LOG(LogGamekit, Log, TEXT("Found FoW Parameter collection"));
        FogMaterialParameters = FoWParameterCollection.Object;
    }
    else
    {
        UE_LOG(LogGamekit, Warning, TEXT("Could not find FoW Parameter collection"));
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> FoWUnostructedVisionMaterial(
            TEXT("Material'/Gamekit/FogOfWar/UnobstructedVisionMaterial.UnobstructedVisionMaterial'"));

    if (FoWUnostructedVisionMaterial.Succeeded())
    {
        UnobstructedVisionMaterial = FoWUnostructedVisionMaterial.Object;
    }
    else
    {
        UE_LOG(LogGamekit, Warning, TEXT("Could not find UnobstructedVision Material"));
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> FoWPostProcessMaterial(
            TEXT("Material'/Gamekit/FogOfWar/FoWPostProcess.FoWPostProcess'"));

    if (FoWPostProcessMaterial.Succeeded())
    {
        BasePostProcessMaterial = FoWPostProcessMaterial.Object;
    }
    else
    {
        UE_LOG(LogGamekit, Warning, TEXT("Could not find Default Post Process Material"));
    }

    GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    GetBrushComponent()->Mobility = EComponentMobility::Static;

    TextureScale             = 1.f;
    FramePerSeconds          = 30.f;
    EnableExploration        = true;
    FogOfWarCollisionChannel = DEFAULT_FoW_COLLISION;
    UseFoWDecalRendering     = true;
    bFoWEnabled              = true;
    FogVersion               = 1;
    bDebug                   = false;
    Margin                   = 25.f;

    DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> FoWPostDecalMaterial(
            TEXT("Material'/Gamekit/FogOfWar/FoWAsDecal.FoWAsDecal'"));

    if (FoWPostProcessMaterial.Succeeded())
    {
        DecalComponent->SetDecalMaterial(FoWPostDecalMaterial.Object);
    }
    else
    {
        UE_LOG(LogGamekit, Warning, TEXT("Could not find Default Decal Material Material"));
    }
}

void AGKFogOfWarVolume::SetFogOfWarMaterialParameters(FName                     name,
                                                      UMaterialInstanceDynamic *Material,
                                                      bool                      CreateRenderTarget)
{
    if (Material == nullptr)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Material is null"));
        return;
    }

    auto FoWView = GetFactionRenderTarget(name, CreateRenderTarget);
    if (FoWView != nullptr)
    {
        Material->SetTextureParameterValue("FoWView", FoWView);
    }

    auto FoWExploration = GetFactionExplorationRenderTarget(name, CreateRenderTarget);
    if (FoWExploration != nullptr)
    {
        Material->SetTextureParameterValue("FoWExploration", FoWExploration);
    }
}

UMaterialInterface *AGKFogOfWarVolume::GetFogOfWarPostprocessMaterial(FName name, bool CreateRenderTarget)
{
    if (UseFoWDecalRendering)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Cannot generate Post process material when using Decal Rendering"));
        return nullptr;
    }

    if (BasePostProcessMaterial == nullptr)
    {
        UE_LOG(LogGamekit, Warning, TEXT("No base post process material to create FoW material"));
        return nullptr;
    }

    UMaterialInterface **LookupResult = PostProcessMaterials.Find(name);

    if (LookupResult != nullptr)
    {
        return LookupResult[0];
    }

    // Those calls will create the render target if missing
    auto FoWView        = GetFactionRenderTarget(name, CreateRenderTarget);
    auto FoWExploration = GetFactionExplorationRenderTarget(name, CreateRenderTarget);

    if (FoWView == nullptr)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Missing FoWView to create FoW post process material"));
        return nullptr;
    }

    UMaterialInstanceDynamic *Material = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
            GetWorld(), BasePostProcessMaterial, NAME_None, EMIDCreationFlags::None);

    if (Material == nullptr)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Could not create FoW post process material"));
        return nullptr;
    }

    if (FoWView != nullptr)
    {
        Material->SetTextureParameterValue("FoWView", FoWView);
    }

    if (FoWExploration != nullptr)
    {
        Material->SetTextureParameterValue("FoWExploration", FoWExploration);
    }

    PostProcessMaterials.Add(name, Material);
    return Material;
}

void AGKFogOfWarVolume::UpdateVolumeSizes()
{
    FScopeLock ScopeLock(&Mutex);

    GetBrushSizes(TextureSize, MapSize);
    SetMapSize(MapSize);
    SetTextureSize(TextureSize);

    // Resize existing Targets to match expectation
    for (auto &RenderTargets: FogFactions)
    {
        RenderTargets.Value->ResizeTarget(TextureSize.X, TextureSize.Y);
        RenderTargets.Value->bNeedsTwoCopies = true;
    }
}

void AGKFogOfWarVolume::GetBrushSizes(FVector2D &TextureSize_, FVector2D &MapSize_)
{
    auto bb   = GetComponentsBoundingBox(true);
    auto Size = bb.GetSize();

    MapSize_ = FVector2D(Size.X, Size.Y);

    if (MapSize_.X == 0 || MapSize_.Y == 0)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Map size too small (%.2f x %.2f)"), MapSize_.X, MapSize_.Y);

        // If size == 0 it will trigger assert on the RHI side
        MapSize_.X = 100.f;
        MapSize_.Y = 100.f;
    }

    if (TextureScale == 0)
    {
        UE_LOG(LogGamekit, Warning, TEXT("TextureScale cannot be zero"));

        // If size == 0 it will trigger assert on the RHI side
        TextureScale = 1.f;
    }

    TextureSize_.X = MapSize_.X * TextureScale;
    TextureSize_.Y = MapSize_.Y * TextureScale;
}

void AGKFogOfWarVolume::InitDecalRendering()
{
    DecalComponent->SetVisibility(UseFoWDecalRendering);

    if (UseFoWDecalRendering && Faction.IsValid())
    {
        auto bb   = GetComponentsBoundingBox(true);
        auto Size = bb.GetSize();

        // UE_LOG(LogGamekit, Warning, TEXT("Resizing DecalComponent %s"), *Size.ToString());
        // DecalComponent->AddLocalRotation(FQuat(FVector(0, 1, 0), -90));
        // DecalComponent->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));

        // Size returns 4000x4000x4000
        // but in the editor a decal size of 100x100x100 is enough
        // The scale of the parent is 20x20x20
        // 200x200x200 would be the unscaled probably need to device it by 2 again

        DecalComponent->DecalSize = Size;
        if (DecalMaterialInstance == nullptr)
        {
            DecalMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
                    GetWorld(), DecalComponent->GetDecalMaterial(), NAME_None, EMIDCreationFlags::None);
            DecalComponent->SetDecalMaterial(DecalMaterialInstance);
        }

        SetFogOfWarMaterialParameters(Faction, DecalMaterialInstance);
    }
}

void AGKFogOfWarVolume::Tick(float DeltaTime)
{
    if (bFoWEnabled)
    {
        DrawFactionFog();
    }
}

void AGKFogOfWarVolume::BeginPlay()
{
    Super::BeginPlay();

    ClearExploration();

    UpdateVolumeSizes();

    // Reset the material instance
    DecalMaterialInstance = nullptr;
    InitDecalRendering();
    SetFoWEnabledParameter(bFoWEnabled);

    // Start drawing the fog
    // For this method to work we need a better way to synchronize the textures
    /*
    GetWorldTimerManager().SetTimer(FogComputeTimer,
                                    this,
                                    &AGKFogOfWarVolume::DrawFactionFog,
                                    1.f / FramePerSeconds,
                                    true,
                                    bFoWEnabled ? 0.f : 1.f);
    
    if (!bFoWEnabled)
    {
        GetWorldTimerManager().PauseTimer(FogComputeTimer);
        return;
    }
    //*/
}

UCanvasRenderTarget2D *AGKFogOfWarVolume::GetFactionRenderTarget(FName name, bool CreateRenderTarget)
{
    UCanvasRenderTarget2D **renderResult = FogFactions.Find(name);
    UCanvasRenderTarget2D * render       = nullptr;

    if (renderResult != nullptr)
    {
        render = renderResult[0];
    }
    else if (CreateRenderTarget)
    {
        GetBrushSizes(TextureSize, MapSize);

        UE_LOG(LogGamekit,
               Log,
               TEXT("Creating a new render target for %s (%.2f x %.2f)"),
               *name.ToString(),
               TextureSize.X,
               TextureSize.Y);

        render = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(
                GetWorld(), UCanvasRenderTarget2D::StaticClass(), TextureSize.X, TextureSize.Y);

        // render->MipGenSettings = TMGS_NoMipmaps;
        FogFactions.Add(name, render);
    }

    render->bNeedsTwoCopies = true;
    return render;
}

UCanvasRenderTarget2D *AGKFogOfWarVolume::GetFactionExplorationRenderTarget(FName name, bool CreateRenderTarget)
{
    UCanvasRenderTarget2D **renderResult = Explorations.Find(name);
    UCanvasRenderTarget2D * render       = nullptr;

    if (!EnableExploration)
    {
        return nullptr;
    }

    if (renderResult != nullptr)
    {
        render = renderResult[0];
    }
    else if (CreateRenderTarget)
    {
        GetBrushSizes(TextureSize, MapSize);

        UE_LOG(LogGamekit,
               Log,
               TEXT("Creating a new render target for %s (%.2f x %.2f)"),
               *name.ToString(),
               TextureSize.X,
               TextureSize.Y);

        render = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(
                GetWorld(), UCanvasRenderTarget2D::StaticClass(), TextureSize.X, TextureSize.Y);

        // render->MipGenSettings = TMGS_NoMipmaps;
        Explorations.Add(name, render);
    }

    render->bNeedsTwoCopies = true;
    return render;
}

UMaterialParameterCollection *AGKFogOfWarVolume::GetMaterialParameterCollection() { return FogMaterialParameters; }

FLinearColor AGKFogOfWarVolume::GetColor()
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return FLinearColor();
    }

    FCollectionVectorParameter const *value = MaterialParameters->GetVectorParameterByName("Color");
    return value->DefaultValue;
}

FLinearColor AGKFogOfWarVolume::GetTextureSize()
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return FLinearColor();
    }

    FCollectionVectorParameter const *value = MaterialParameters->GetVectorParameterByName("TextureSize");
    return value->DefaultValue;
}

FLinearColor AGKFogOfWarVolume::GetMapSize()
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return FLinearColor();
    }

    FCollectionVectorParameter const *value = MaterialParameters->GetVectorParameterByName("MapSize");
    return value->DefaultValue;
}

void AGKFogOfWarVolume::SetColor(FLinearColor color)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return;
    }

    UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), MaterialParameters, "Color", color);
}

void AGKFogOfWarVolume::SetTextureSize(FVector2D size)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return;
    }

    TextureSize = size;
    UKismetMaterialLibrary::SetVectorParameterValue(
            GetWorld(), MaterialParameters, "TextureSize", FLinearColor(size.X, size.Y, 1.f, 1.f));
}

void AGKFogOfWarVolume::SetFoWEnabledParameter(bool Enabled)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return;
    }

    bFoWEnabled = Enabled;
    UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MaterialParameters, "FoWEnabled", float(bFoWEnabled));
}

void AGKFogOfWarVolume::SetMapSize(FVector2D size)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return;
    }

    MapSize = size;
    UKismetMaterialLibrary::SetVectorParameterValue(
            GetWorld(), MaterialParameters, "MapSize", FLinearColor(size.X, size.Y, 1.f, 1.f));
}

void AGKFogOfWarVolume::RegisterActorComponent(class UGKFogOfWarComponent *c)
{
    FScopeLock ScopeLock(&Mutex);
    ActorComponents.Add(c);
}

void AGKFogOfWarVolume::UnregisterActorComponent(class UGKFogOfWarComponent *c)
{
    FScopeLock ScopeLock(&Mutex);
    ActorComponents.Remove(c);
}

void AGKFogOfWarVolume::DrawFactionFog()
{
    // We are drawing to the targets we cannot change the fog components right now
    FScopeLock ScopeLock(&Mutex);

    for (auto &RenderTargets: FogFactions)
    {
        UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTargets.Value);
    }

    for (auto &Component: ActorComponents)
    {
        DrawLineOfSight(Component);
    }

    // Update exploration texture if any
    UpdateExploration();
}

void AGKFogOfWarVolume::DrawLineOfSight(UGKFogOfWarComponent *c)
{
    if (!c->GivesVision)
    {
        return;
    }

    if (c->UnobstructedVision)
    {
        DrawUnobstructedLineOfSight(c);
    }
    else
    {
        DrawObstructedLineOfSight(c);
    }
}

void BroadCastEvents(AActor *Seer, UGKFogOfWarComponent *SeerComponent, AActor *Target)
{
    // Send an event that current actor is seeing something
    SeerComponent->OnSighting.Broadcast(Target);

    UActorComponent *component = Target->GetComponentByClass(UGKFogOfWarComponent::StaticClass());
    if (component == nullptr)
    {
        return;
    }

    UGKFogOfWarComponent *target = Cast<UGKFogOfWarComponent>(component);
    if (target == nullptr)
    {
        return;
    }

    // Send an event that the hit is being seen
    target->OnSighted.Broadcast(Seer);
}

void AGKFogOfWarVolume::DrawUnobstructedLineOfSight(UGKFogOfWarComponent *c)
{
    AActor *actor   = c->GetOwner();
    FVector forward = actor->GetActorForwardVector();

    UMaterialInstanceDynamic *material = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
            GetWorld(), UnobstructedVisionMaterial, NAME_None, EMIDCreationFlags::None);

    FLinearColor Value;
    Value.R = forward.X;
    Value.G = forward.Y;
    Value.B = c->FieldOfView;
    Value.A = c->InnerRadius / c->Radius;
    material->SetVectorParameterValue("Direction&FoV", Value);

    auto RenderCanvas = GetFactionRenderTarget(c->Faction, true);
    auto NewRadius    = FVector2D(c->Radius * TextureSize.X / MapSize.X, c->Radius * TextureSize.Y / MapSize.Y);
    auto Start        = GetTextureCoordinate(actor->GetActorLocation()) - NewRadius;

    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;
    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderCanvas, Canvas, Size, Context);

    Canvas->K2_DrawMaterial(Cast<UMaterialInterface>(material),
                            Start,
                            NewRadius * 2.f,
                            FVector2D(0.f, 0.f),
                            FVector2D::UnitVector,
                            0.f,
                            FVector2D(0.5f, 0.5f));

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);

    TArray<AActor *>                      ActorsToIgnore;
    UClass *                              ActorClassFilter = AActor::StaticClass();
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    UGKFogOfWarLibrary::ConvertToObjectType(FogOfWarCollisionChannel, ObjectTypes);
    TArray<AActor *>                      OutActors;

    UKismetSystemLibrary::SphereOverlapActors(
            GetWorld(), actor->GetActorLocation(), c->Radius, ObjectTypes, ActorClassFilter, ActorsToIgnore, OutActors);

    for (AActor *Target: OutActors)
    {
        if (Target == nullptr)
        {
            continue;
        }

        BroadCastEvents(actor, c, Target);
    }
}


void AGKFogOfWarVolume::DrawObstructedLineOfSight(class UGKFogOfWarComponent *c) {
    AActor * actor = c->GetOwner();
    FVector  loc   = actor->GetActorLocation();

    if (bDebug)
    {
        UKismetSystemLibrary::DrawDebugCircle(
            GetWorld(),          // World
            loc,                 // Center
            c->Radius,           // Radius
            c->TraceCount,       // NumSegments
            FLinearColor::White, // LineColor
            0.f,                 // LifeTime 
            5.f,                 // Tickness
            FVector(1, 0, 0),    // YAxis 
            FVector(0, 1, 0),    // ZAxis 
            true                 // DrawAxes
        );
    }


    switch (FogVersion)
    {
    case 1:  return DrawObstructedLineOfSight_RayCastV1(c);
    case 2:  return DrawObstructedLineOfSight_RayCastV2(c);
    case 3:  return DrawObstructedLineOfSight_RayCastV3(c);
    default: return DrawObstructedLineOfSight_RayCastV2(c);
    }
}

void AGKFogOfWarVolume::DrawObstructedLineOfSight_RayCastV3(UGKFogOfWarComponent *c) {
    Triangles.Reset(c->TraceCount + 1);

    TArray<AActor *>                      ActorsToIgnore   = {
        c->GetOwner()
    };
    UClass *                              ActorClassFilter = AActor::StaticClass();
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    UGKFogOfWarLibrary::ConvertToObjectType(FogOfWarCollisionChannel, ObjectTypes);

    AActor *                              Actor            = c->GetOwner();
    TArray<AActor *>                      OutActors;

    FVector Location = Actor->GetActorLocation();
    FVector Forward  = Actor->GetActorForwardVector();

    /*
    OverlapMultiByObjectType(
            TArray< struct FOverlapResult > & OutOverlaps,
            const FVector & Pos,
            const FQuat & Rot,
            const FCollisionObjectQueryParams & ObjectQueryParams,
            const FCollisionShape & CollisionShape,
            const FCollisionQueryParams & Params
        ) const;

        */
    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(), 
        Location, 
        c->Radius, 
        ObjectTypes, 
        ActorClassFilter,
        ActorsToIgnore, 
        OutActors
    );

    UE_LOG(LogGamekit, Log, TEXT("Found %d Actors"), OutActors.Num());

    TArray<FVector> EndPoints;
    TArray<float> Angles;
    FVector Origin;
    FVector BoxExtent;
    FVector Cartesian;
    auto BaseYaw = Actor->GetActorRotation().Yaw;

    auto AddPointAsAngle = [&Angles, &Location, &BaseYaw](FVector Point) {
        auto Angle = UGKUtilityLibrary::GetYaw(Location, Point) - BaseYaw;
        Angles.Add(Angle);
    };

    for (auto &OutActor: OutActors)
    {
        OutActor->GetActorBounds(true, Origin, BoxExtent);

        float Hypotenuse = (Origin - Location).Size2D();
        float Adjacent   = (BoxExtent.X + BoxExtent.Y) / 2.f;
        float Angle      = FMath::Acos(Adjacent / Hypotenuse);

        UE_LOG(LogGamekit, Log, TEXT("Actor %s %f"), *AActor::GetDebugName(OutActor), FMath::RadiansToDegrees(Angle));

        Origin.Z = Location.Z;
        auto TargetYaw = FMath::DegreesToRadians(UGKUtilityLibrary::GetYaw(Origin, Location));

        // 1st ray is a bit outside
        FMath::PolarToCartesian(Adjacent + Margin, TargetYaw + Angle, Cartesian.X, Cartesian.Y);
        AddPointAsAngle(Origin + Cartesian);

        // 2nd ray is hiting the actor
        FMath::PolarToCartesian(Adjacent - Margin, TargetYaw + Angle, Cartesian.X, Cartesian.Y);
        AddPointAsAngle(Origin + Cartesian);

        // Middle ray for good measure ?
        AddPointAsAngle(Origin);

        // 3rd ray is hiting the actor
        FMath::PolarToCartesian(Adjacent - Margin, TargetYaw - Angle, Cartesian.X, Cartesian.Y);
        AddPointAsAngle(Origin + Cartesian);

        // 4th ray is a bit outside
        FMath::PolarToCartesian(Adjacent + Margin, TargetYaw - Angle, Cartesian.X, Cartesian.Y);
        AddPointAsAngle(Origin + Cartesian);
    }

    //*
    float step = c->FieldOfView / float(c->TraceCount);
    int   n    = c->TraceCount / 2;

    for (int i = -n; i <= n; i++)
    {
        float angle = float(i) * step;
        Angles.Add(angle);
    }

    Angles.Sort();
    //*/

    /*
    // Make sure the angles are not too far appart
    float step = c->FieldOfView / float(c->TraceCount);

    Angles.Sort();
    TArray<float> NewAngles;
    float         Previous = Angles[0];
    NewAngles.Add(Previous);
    Angles.Add(Previous);

    for (int i = 1; i < Angles.Num(); i++)
    {
        auto Angle = Angles[i];
        auto Range = (Angle - Previous);
        auto Diff = int(Range / step);

        for (int j = 1; j < Diff; j++)
        {
            NewAngles.Add(Previous + Range / float(Diff) * j);
        }

        NewAngles.Add(Angle);
    }
    //*/

    GenerateTrianglesFromAngles(c, Angles);

    DrawTriangles(c);
}

void AGKFogOfWarVolume::GenerateTrianglesFromAngles(UGKFogOfWarComponent *c, TArray<float>& Angles) {
    auto Actor       = c->GetOwner();
    FVector Location = Actor->GetActorLocation();
    FVector Forward  = Actor->GetActorForwardVector();

    TArray<AActor *> ActorsToIgnore   = {};

    FCanvasUVTri Triangle;
    FHitResult OutHit;
    auto TraceType = UEngineTypes::ConvertToTraceType(FogOfWarCollisionChannel);
    FVector2D    Previous;
    FVector      LinePrevious;
    bool         bHasPrevious = false;
    auto TriangleSize = FVector2D(c->Radius, c->Radius) * 2.f;
    TSet<AActor *>   AlreadySighted;

    for (auto Angle: Angles)
    {
        // We need to use forward vector in case the FieldOfView is not 360
        FVector dir = Forward.RotateAngleAxis(Angle, FVector(0, 0, 1));

        // We have to ignore the Inner Radius for our triangles to be
        // perfectly completing each other
        FVector LineStart = Location; // + dir * c->InnerRadius;
        FVector LineEnd   = Location + dir * c->Radius;

        if (bDebug)
        {
            UKismetSystemLibrary::DrawDebugCircle(
                GetWorld(),          // World
                LineEnd,             // Center
                25.f,                // Radius
                36,                  // NumSegments
                FLinearColor::White, // LineColor
                0.f,                 // LifeTime 
                5.f,                 // Tickness
                FVector(1, 0, 0),    // YAxis 
                FVector(0, 1, 0),    // ZAxis 
                true                 // DrawAxes
            );
        }

        bool hit = UKismetSystemLibrary::LineTraceSingle(
            GetWorld(),
            Location,
            LineEnd,
            TraceType,
            false, // bTraceComplex
            ActorsToIgnore,
            DebugTrace(),
            OutHit,
            true, // Ignore Self
            FLinearColor::Red,
            FLinearColor::Green,
            5.0f
        );

        LineEnd = hit ? OutHit.Location : LineEnd;

        auto Start = GetTextureCoordinate(LineStart);

        // FIXME: Effective radius turns out to be a bit smaller
        auto End   = GetTextureCoordinate(LineEnd);

        //Triangle.V0_Color = FLinearColor::White;
        Triangle.V0_Pos   = Start;
        Triangle.V0_UV  = FVector2D(0.5, 0.5);

        //Triangle.V1_Color = FLinearColor::White;
        Triangle.V1_Pos   = Previous;
        Triangle.V1_UV    = UGKCoordinateLibrary::ToTextureCoordinate((LinePrevious - LineStart), TriangleSize);

        //Triangle.V2_Color = FLinearColor::White;
        Triangle.V2_Pos   = End;
        Triangle.V2_UV    = UGKCoordinateLibrary::ToTextureCoordinate((LineEnd - LineStart), TriangleSize);

        if (bHasPrevious)
        {
            Triangles.Add(Triangle);
        }

        bHasPrevious = true;
        Previous = End;
        LinePrevious = LineEnd;
        
        if (hit && OutHit.Actor.IsValid())
        {
            // Avoid multiple broadcast per target
            AActor *Target = OutHit.Actor.Get();
            if (!AlreadySighted.Contains(Target))
            {
                AlreadySighted.Add(Target);
                BroadCastEvents(Actor, c, Target);
            }
        }
    }
}

void AGKFogOfWarVolume::DrawTriangles(UGKFogOfWarComponent *c) {
    // Draw all Triangles
    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    auto RenderCanvas = GetFactionRenderTarget(c->Faction, true);
    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderCanvas, Canvas, Size, Context);

    Canvas->K2_DrawMaterialTriangle(TrianglesMaterial, Triangles);

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
    Triangles.Reset(c->TraceCount + 1);
}

// This is slower than V1, tracce the same amount of rays
// We could improve this by only focusing the rays around the relevant actors
// Make a polygon and draw the result
void AGKFogOfWarVolume::DrawObstructedLineOfSight_RayCastV2(UGKFogOfWarComponent *c) {
    Triangles.Reset(c->TraceCount + 1);

    AActor *         actor          = c->GetOwner();
    FVector          forward        = actor->GetActorForwardVector();
    FVector          loc            = actor->GetActorLocation();
    TArray<AActor *> ActorsToIgnore = {GetOwner()};
    TSet<AActor *>   AlreadySighted;

    float step = c->FieldOfView / float(c->TraceCount);
    int   n    = c->TraceCount / 2;

    FHitResult   OutHit;
    FCanvasUVTri Triangle;
    auto         TraceType = UEngineTypes::ConvertToTraceType(FogOfWarCollisionChannel);
    FVector2D    Previous;
    FVector      LinePrevious;
    bool         bHasPrevious = false;

    // Because we are drawing triangles but our field of view is arched 
    // we make the radius bigger the circle outline will be drawn using the texture
    float halfStep       = step / 2.f;
    float ExtendedRadius = FMath::Sqrt(
        FMath::Square(c->Radius / FMath::Tan(FMath::DegreesToRadians(90.f - halfStep)))
        + FMath::Square(c->Radius)
    );
    
    auto TriangleSize = FVector2D(c->Radius, c->Radius) * 2.f;

    // UE_LOG(LogGamekit, Log, TEXT("Extended Radius is %f (%f)"), ExtendedRadius, c->Radius);

    for (int i = -n; i <= n; i++)
    {
        float   angle = float(i) * step;
        FVector dir   = forward.RotateAngleAxis(angle, FVector(0, 0, 1));

        // We have to ignore the Inner Radius for our triangles to be
        // perfectly completing each other
        FVector LineStart = loc; // + dir * c->InnerRadius;
        FVector LineEnd   = loc + dir * c->Radius;

        bool hit = UKismetSystemLibrary::LineTraceSingle(GetWorld(),
                                                         LineStart,
                                                         LineEnd,
                                                         TraceType,
                                                         false, // bTraceComplex
                                                         ActorsToIgnore,
                                                         DebugTrace(),
                                                         OutHit,
                                                         true, // Ignore Self
                                                         FLinearColor::Red,
                                                         FLinearColor::Green,
                                                         5.0f);

        LineEnd = hit ? OutHit.Location : loc + dir * ExtendedRadius;

        auto Start = GetTextureCoordinate(LineStart);

        // FIXME: Effective radius turns out to be a bit smaller
        auto End   = GetTextureCoordinate(LineEnd);

        //Triangle.V0_Color = FLinearColor::White;
        Triangle.V0_Pos   = Start;
        Triangle.V0_UV  = FVector2D(0.5, 0.5);

        //Triangle.V1_Color = FLinearColor::White;
        Triangle.V1_Pos   = Previous;
        Triangle.V1_UV    = UGKCoordinateLibrary::ToTextureCoordinate((LinePrevious - LineStart), TriangleSize);

        //Triangle.V2_Color = FLinearColor::White;
        Triangle.V2_Pos   = End;
        Triangle.V2_UV    = UGKCoordinateLibrary::ToTextureCoordinate((LineEnd - LineStart), TriangleSize);

        if (bHasPrevious)
        {
            Triangles.Add(Triangle);
        }

        bHasPrevious = true;
        Previous = End;
        LinePrevious = LineEnd;
        
        if (hit && OutHit.Actor.IsValid())
        {
            // Avoid multiple broadcast per target
            AActor *Target = OutHit.Actor.Get();
            if (!AlreadySighted.Contains(Target))
            {
                AlreadySighted.Add(Target);
                BroadCastEvents(actor, c, Target);
            }
        }
    }

    DrawTriangles(c);
}

void AGKFogOfWarVolume::DrawObstructedLineOfSight_RayCastV1(UGKFogOfWarComponent *c)
{
    AActor *         actor          = c->GetOwner();
    FVector          forward        = actor->GetActorForwardVector();
    FVector          loc            = actor->GetActorLocation();
    TArray<AActor *> ActorsToIgnore = {GetOwner()};
    TSet<AActor *>   AlreadySighted;

    float step = c->FieldOfView / float(c->TraceCount);
    int   n    = c->TraceCount / 2;

    FHitResult                 OutHit;
    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    auto RenderCanvas = GetFactionRenderTarget(c->Faction, true);
    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderCanvas, Canvas, Size, Context);
    auto TraceType = UEngineTypes::ConvertToTraceType(FogOfWarCollisionChannel);

    for (int i = -n; i <= n; i++)
    {
        float   angle = float(i) * step;
        FVector dir   = forward.RotateAngleAxis(angle, FVector(0, 0, 1));

        FVector LineStart = loc + dir * c->InnerRadius;
        FVector LineEnd   = loc + dir * c->Radius;

        bool hit = UKismetSystemLibrary::LineTraceSingle(GetWorld(),
                                                         LineStart,
                                                         LineEnd,
                                                         TraceType,
                                                         false, // bTraceComplex
                                                         ActorsToIgnore,
                                                         DebugTrace(),
                                                         OutHit,
                                                         true, // Ignore Self
                                                         FLinearColor::Red,
                                                         FLinearColor::Green,
                                                         5.0f);

        LineEnd = hit ? OutHit.Location : LineEnd;

        auto Start = GetTextureCoordinate(LineStart);
        auto End   = GetTextureCoordinate(LineEnd);

        //
        //
        Canvas->K2_DrawLine(Start, End, c->LineTickness, FLinearColor::White);

        if (hit && OutHit.Actor.IsValid())
        {
            // Avoid multiple broadcast per target
            AActor *Target = OutHit.Actor.Get();
            if (!AlreadySighted.Contains(Target))
            {
                AlreadySighted.Add(Target);
                BroadCastEvents(actor, c, Target);
            }
        }
    }

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
}

void AGKFogOfWarVolume::ClearExploration()
{
    for (auto &RenderTargets: Explorations)
    {
        UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTargets.Value);
    }
}

void AGKFogOfWarVolume::UpdateExploration()
{
    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    for (auto &RenderTargets: Explorations)
    {
        auto ExpFaction    = RenderTargets.Key;
        auto Exploration   = RenderTargets.Value;
        auto CurrentVision = GetFactionRenderTarget(ExpFaction, false);

        // this check might not be necessary
        if (CurrentVision == nullptr)
        {
            continue;
        }

        UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), Exploration, Canvas, Size, Context);
        Canvas->K2_DrawTexture(CurrentVision,
                               FVector2D(0, 0),
                               Size,
                               FVector2D(0, 0),
                               FVector2D(1, 1),
                               FLinearColor(0, 1, 0, 0),
                               EBlendMode::BLEND_Additive,
                               0.0,
                               FVector2D(0, 0));
        UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
    }
}
