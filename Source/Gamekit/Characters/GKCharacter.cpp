// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Characters/GKCharacter.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilityInputs.h"
#include "Gamekit/Abilities/GKAbilityStatic.h"
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"
#include "Gamekit/Abilities/GKGameplayAbility.h"
#include "Gamekit/Abilities/GKAbilityInterface.h"
#include "Gamekit/GKLog.h"
#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"
#include "Gamekit/FogOfWar/GKFogOfWarLibrary.h"

// Unreal Engine
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/NetworkObjectList.h"
#include "Engine/ActorChannel.h"


AGKCharacterBase::AGKCharacterBase()
{
    // Create ability system component, and set it to be explicitly replicated
    AbilitySystemComponent = CreateDefaultSubobject<UGKAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);

    // Create the attribute set, this replicates by default
    AttributeSet = CreateDefaultSubobject<UGKAttributeSet>(TEXT("AttributeSet"));

    CharacterLevel = 1;
    InputsBound    = false;
    NetDormancy    = DORM_DormantPartial;
    bPreviousVisibility = true;
}


void AGKCharacterBase::Tick(float Delta) {
}

void AGKCharacterBase::OnReplicationPausedChanged(bool bIsReplicationPaused) {
    // Stop the current interpolation to continue, since we are not going to receive
    // position updates
    //
    // Tempering with this would only cause the client to see ghost actors
    if (bIsReplicationPaused){
        Cast<UCharacterMovementComponent>(GetMovementComponent())->DisableMovement();
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
        SetActorTickEnabled(false);
    } else {
        SetActorHiddenInGame(false);
        SetActorEnableCollision(true);
        SetActorTickEnabled(true);
    }
}


bool AGKCharacterBase::IsReplicationPausedForConnection(const FNetViewer& ConnectionOwnerNetViewer) {
    if (ConnectionOwnerNetViewer.ViewTarget == this)
        return false;
    
    //*
    auto FogComp = Cast<UGKFogOfWarComponent>(GetComponentByClass(UGKFogOfWarComponent::StaticClass()));

    if (FogComp)
    {
        auto ViewTarget = ConnectionOwnerNetViewer.ViewTarget;

        auto bVisible = FogComp->IsVisible(ViewTarget);

        return !bVisible;
    }
    //*/

    return false;
}

void AGKCharacterBase::OnDataTableChanged_Native()
{
    // Reset Cache
    UnitStatic = nullptr;
    GetUnitStatic();
}

void AGKCharacterBase::PostInitProperties()
{
    Super::PostInitProperties();
}

void AGKCharacterBase::AddPassiveEffect(UGameplayEffect *Effect)
{
    if (Effect == nullptr)
    {
        return;
    }

    FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    auto                        ManaSpec     = FGameplayEffectSpec(Effect, EffectContext, 1);
    FActiveGameplayEffectHandle ActiveHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(ManaSpec);

    // NOTE: in examples:
    // FGameplayEffectSpecHandle ManaSpecSpecHandle = new FGameplayEffectSpec(Effect, EffectContext, 1);

    // ManaSpecSpecHandle is a shared pointer not sure if this is really necessary
    // could be referenced somewhere at somepoint but until proven necessary do not make a shared pointer

    // ApplyGameplayEffectSpecToTarget just calls ApplyGameplayEffectSpecToSelf
    // FActiveGameplayEffectHandle ActiveGEHandle =
    // AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*ManaSpecSpecHandle.Data.Get(), AbilitySystemComponent);
}

void AGKCharacterBase::LoadFromDataTable(FGKUnitStatic &UnitDef)
{
    if (GetLocalRole() != ROLE_Authority || AbilitySystemComponent == nullptr)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Not Authority"));
        return;
    }

    if (!AttributeSet || AbilitySystemComponent->IsInitialized())
    {
        return;
    }

    AttributeSet->SetMaxHealth(UnitDef.Health);
    AttributeSet->SetMaxMana(UnitDef.Mana);

    auto ManaRegen = NewPassiveRegenEffect(
        AbilitySystemComponent, 
        UGKAttributeSet::GetManaAttribute(), 
        UnitDef.ManaRegen, 
        0.1
        //, TEXT("GameplayEffect.ManaRegen")
    );
    AddPassiveEffect(ManaRegen);

    auto HealthRegen = NewPassiveRegenEffect(
        AbilitySystemComponent, 
        UGKAttributeSet::GetHealthAttribute(), 
        UnitDef.HealthRegen, 
        0.1
        //, TEXT("GameplayEffect.HealthRegen")
    );
    AddPassiveEffect(HealthRegen);

    // Grant Initial Abilities
    // Need to disable level 0 abilities now
    for (TSubclassOf<UGKGameplayAbility> &AbilityClass: UnitDef.Abilities)
    {
        FGameplayAbilitySpec Spec(
                AbilityClass, // TSubclassOf<UGameplayAbility> |
                0,            // int32 InLevel					|
                INDEX_NONE,   // int32 InInputID				| TODO: This is user configured
                nullptr       // UObject * InSourceObject		|
        );

        AbilitySystemComponent->GiveAbility(Spec);
    }
    // ---

    AbilitySystemComponent->Initialized = true;
}

void AGKCharacterBase::K2_GetUnitStatic(FGKUnitStatic &UnitStaticOut, bool &Valid)
{
    Valid = false;

    auto Result = GetUnitStatic();
    if (Result != nullptr)
    {
        UnitStaticOut = *Result;
        Valid         = true;
    }
}

FGKUnitStatic *AGKCharacterBase::GetUnitStatic()
{
    // We have already a cached lookup
    if (UnitStatic)
    {
        return UnitStatic;
    }

    if (!UnitDataTable || !UnitRowName.IsValid())
    {
        UE_LOG(LogGamekit, Warning, TEXT("Unit not configured to use DataTable"));
        return nullptr;
    }

    UnitStatic = UnitDataTable->FindRow<FGKUnitStatic>(UnitRowName, TEXT("Unit"), true);

    if (UnitStatic != nullptr)
    {
        LoadFromDataTable(*UnitStatic);
    }
    else
    {
        UE_LOG(LogGamekit, Warning, TEXT("Did not find  Unit static"));
    }

    // Listen to data table change
    if (!UnitDataTable->OnDataTableChanged().IsBoundToObject(this))
    {
        UnitDataTable->OnDataTableChanged().AddUObject(this, &AGKCharacterBase::OnDataTableChanged_Native);
    }

    return UnitStatic;
}

UAbilitySystemComponent *AGKCharacterBase::GetAbilitySystemComponent() const { return AbilitySystemComponent; }

void AGKCharacterBase::OnHealthChanged_Native(const FOnAttributeChangeData &Data)
{

    // Health gets clamp to 0 so we know that it will be exactly equal to 0
    if (Data.NewValue == 0.f)
    {
        Die();
    }
}

void AGKCharacterBase::ClearGameplayAbilities()
{
    if (GetLocalRole() != ROLE_Authority || AbilitySystemComponent == nullptr || !AbilitySystemComponent->Initialized)
    {
        return;
    }

    TArray<FGameplayAbilitySpecHandle> Abilities;

    auto ASC = GetAbilitySystemComponent();
    for (const FGameplayAbilitySpec &Spec: ASC->GetActivatableAbilities())
    {
        if ((Spec.SourceObject == this))
        {
            Abilities.Add(Spec.Handle);
        }
    }

    for (auto &Ability: Abilities)
    {
        AbilitySystemComponent->ClearAbility(Ability);
    }

    AbilitySystemComponent->Initialized = false;
}

void AGKCharacterBase::Die()
{
    // Remove all abilities
    ClearGameplayAbilities();

    // Disable Collision
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->GravityScale = 0;
    GetCharacterMovement()->Velocity     = FVector(0);

    // Clean up the ASC
    const FGameplayTag &DeathDispelTag = Dispel;
    const FGameplayTag &DeathTag       = DisableDead;

    if (AbilitySystemComponent != nullptr)
    {
        AbilitySystemComponent->CancelAllAbilities();

        FGameplayTagContainer EffectTagsToRemove;
        EffectTagsToRemove.AddTag(DeathDispelTag);
        int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);
        AbilitySystemComponent->AddLooseGameplayTag(DeathTag);
    }

    Deselect();

    // Play the death montage
    // NB: we could play it in the animation bp as well
    //*
    if (UnitStatic)
    {
        auto Montage = UnitStatic->AnimationSet.Sample(EGK_AbilityAnimation::Death);
        if (Montage)
        {
            PlayAnimMontage(Montage);
        }
    }
    //*/

    // let blueprints do something about it
    OnDeath_Native();
}

void AGKCharacterBase::OnDeath_Native() {
    ReceiveDeath();
}

bool AGKCharacterBase::IsDead() { return GetAttributeSet()->Health.GetCurrentValue() == 0.f; }

void AGKCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    // Initialize our abilities
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);

        if (UnitDataTable && UnitRowName.IsValid())
        {
            UE_LOG(LogGamekit, Warning, TEXT("Loading Unit Config From DataTable"));
            OnDataTableChanged_Native();
        }

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAttributeSet()->GetHealthAttribute())
                .AddUObject(this, &AGKCharacterBase::OnHealthChanged_Native);
    }
}

void AGKCharacterBase::PossessedBy(AController *NewController)
{
    Super::PossessedBy(NewController);

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->RefreshAbilityActorInfo();
    }

    // ASC MixedMode replication requires that the ASC Owner's Owner be the Controller.
    SetOwner(NewController);
}

void AGKCharacterBase::UnPossessed() {}

void AGKCharacterBase::OnRep_Controller()
{
    Super::OnRep_Controller();

    // Our controller changed, must update ActorInfo on AbilitySystemComponent
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->RefreshAbilityActorInfo();
    }
}

void AGKCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGKCharacterBase, CharacterLevel);

    // this property never changes once it is set by the game mode 
    // COND_InitialOnly
    // but it could be modified on the user side by cheats
    DOREPLIFETIME(AGKCharacterBase, Team);
}

int32 AGKCharacterBase::GetCharacterLevel() const { return CharacterLevel; }

bool AGKCharacterBase::SetCharacterLevel(int32 NewLevel)
{
    if (CharacterLevel != NewLevel && NewLevel > 0)
    {
        // Our level changed so we need to refresh abilities
        CharacterLevel = NewLevel;
        return true;
    }
    return false;
}

void AGKCharacterBase::GrantAbility(FGKAbilitySlot Slot, TSubclassOf<UGKGameplayAbility> AbilityClass, int Level) /* InputID -1 */
{
    // Only called with authority
    // This has input binding that should be customizable
    // 0 is None, The enum for AbilityInput starts at one
    FGameplayAbilitySpec Spec = FGameplayAbilitySpec(
        AbilityClass,
        Level,
        static_cast<int32>(Slot.SlotNumber), // EGK_MOBA_AbilityInputID::Skill1 + offset
        this
    );

    FGameplayAbilitySpecHandle Handle = GetAbilityHandle(Slot);

    if (!Handle.IsValid()) {
        Handle = AbilitySystemComponent->GiveAbility(Spec);
    } else {
        GK_LOG(TEXT("Ability was already granted"));
    }
}

void AGKCharacterBase::RemoveAbility(FGKAbilitySlot Slot) {
    auto Handle = GetAbilityHandle(Slot);
    AbilitySystemComponent->SetRemoveAbilityOnEnd(Handle);
    AbilitySystemComponent->CancelAbilityHandle(Handle);
}

void AGKCharacterBase::OnAbilityRemoved_Native(FGameplayAbilitySpec& AbilitySpec) {
    auto Id = FGKAbilitySlot(AbilitySpec.InputID);

    GK_LOG(TEXT("Removing Ability %s with Input %d: %s"),
        *AbilitySpec.Ability->GetFName().ToString(),
        AbilitySpec.InputID,
        *UEnum::GetValueAsString(EGK_MOBA_AbilityInputID(AbilitySpec.InputID)));

    AbilitySpecs.Remove(Id);
}

void AGKCharacterBase::EquipItem(EGK_ItemSlot Slot, TSubclassOf<UGKGameplayAbility> AbilityClass)
{
    // Only called with authority
    FGameplayAbilitySpec Spec = FGameplayAbilitySpec(AbilityClass,
                                                     1,
                                                     0, // No Input for this
                                                     this);

    // Items get activated as soon as they are equipped
    // NOTE: for items that have actives: the active ability is granted on equip
    //
    FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbilityAndActivateOnce(Spec);
}

UGKGameplayAbility *AGKCharacterBase::GetAbilityInstance(FGKAbilitySlot Slot)
{
    auto Spec = AbilitySpecs.Find(Slot);

    if (Spec == nullptr || Spec->Ability == nullptr)
    {
        return nullptr;
    }

    auto ability = Spec->GetPrimaryInstance();
    if (!ability)
    {
        ability = Spec->Ability;
    }

    // AbilitySystemComponent->execClientActivateAbilityFailed

    return Cast<UGKGameplayAbility>(ability);
}

FGameplayAbilitySpecHandle AGKCharacterBase::GetAbilityHandle(FGKAbilitySlot Slot)
{
    auto Spec = AbilitySpecs.Find(Slot);

    if (Spec == nullptr || Spec->Ability == nullptr)
    {
        return FGameplayAbilitySpecHandle();
    }

    return Spec->Handle;
}

void AGKCharacterBase::Select() { OnSelect(); }

void AGKCharacterBase::Deselect() { OnDeselect(); }

void AGKCharacterBase::OnNewAbility_Native(FGameplayAbilitySpec &AbilitySpec)
{
    // AbilityName is the unique tag per ability
    auto Id = FGKAbilitySlot(AbilitySpec.InputID);

    GK_LOG(TEXT("Adding Ability %s with Input %d: %s"),
           *AbilitySpec.Ability->GetFName().ToString(),
           AbilitySpec.InputID,
           *UEnum::GetValueAsString(EGK_MOBA_AbilityInputID(AbilitySpec.InputID)));

    AbilitySpecs.Add(Id, AbilitySpec);
    ReceiveNewAbility(Id, AbilitySpec);
}

bool AGKCharacterBase::ActivateAbility(FGKAbilitySlot Slot)
{
    auto Spec = AbilitySpecs.Find(Slot);

    if (Spec == nullptr || Spec->Ability == nullptr)
    {
        return false;
    }

    return AbilitySystemComponent->TryActivateAbility(Spec->Handle, true);
}

int AGKCharacterBase::AbilityCount() const { return AbilitySpecs.Num(); }

UGKAttributeSet *AGKCharacterBase::GetAttributeSet() { return AttributeSet; }


void AGKCharacterBase::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (!InputsBound && AbilitySystemComponent != nullptr && IsValid(InputComponent))
    {
        OnSetupPlayerInputComponent(PlayerInputComponent);
        InputsBound = true;
    }
}

void AGKCharacterBase::OnSetupPlayerInputComponent_Implementation(UInputComponent* PlayerInputComponent) {
    if(!InputsBound && AbilitySystemComponent != nullptr && IsValid(InputComponent))
    {
        // This is not that good of a default
        // this assume ConfirmTarget & CancelTarget are defined in the project setting as inputs
        auto GAIB = FGameplayAbilityInputBinds(
            FString("ConfirmTarget"),
            FString("CancelTarget"),
            FString("EGK_MOBA_AbilityInputID")
            // static_cast<int32>(EGK_MOBA_AbilityInputID::Confirm),
            // static_cast<int32>(EGK_MOBA_AbilityInputID::Cancel)
        );

        AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, GAIB);

        GK_WARNING(TEXT("Abilities got bound to inputs using the default logic"));
    }
}

void AGKCharacterBase::BindAbilityActivationFromInputEnum(UInputComponent* PlayerInputComponent, FGKGameplayAbilityInputBinds InputBinds) {
    AbilitySystemComponent->BindAbilityActivationToInputComponentFromEnum(PlayerInputComponent, InputBinds);
}

void AGKCharacterBase::MoveToLocation(FVector Loc)
{
    auto Spec = AbilitySpecs.Find(FGKAbilitySlot(EGK_MOBA_AbilityInputID::Move));

    if (Spec == nullptr || Spec->Ability == nullptr)
    {
        return;
    }

    AbilitySystemComponent->TryActivateAbility_Point(Spec->Handle, Loc);
}

void AGKCharacterBase::SetGenericTeamId(const FGenericTeamId &TeamID) { 
    // Only authority can change the TeamID    
    if (GetNetMode() == ENetMode::NM_Client)
        return;

    Team = TeamID;
    GK_LOG(TEXT("Authority: %s, got assigned team %d"), *GetDebugName(this), Team.GetId());
    ReceiveTeamAssigned(Team);
}

void AGKCharacterBase::OnRep_Team() {
    GK_LOG(TEXT("Client: %s, got assigned team %d"), *GetDebugName(this), Team.GetId());
    ReceiveTeamAssigned(Team);
}

FGenericTeamId AGKCharacterBase::GetGenericTeamId() const { 
    //
    return Team; 
}

