// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.
#pragma once

// Unreal Engine
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectMacros.h"

// Generated
#include "GKAbilityTask_WaitTargetData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGKWaitTargetDataDelegate, const FGameplayAbilityTargetDataHandle &, Data);

/**
 * Fork of UE UAbilityTask_WaitTargetData to enable fully reuasble AbilityTargetActors
 *
 */
UCLASS()
class GAMEKIT_API UGKAbilityTask_WaitForTargetData: public UAbilityTask
{
    GENERATED_UCLASS_BODY()

    UPROPERTY(BlueprintAssignable)
    FGKWaitTargetDataDelegate ValidData;

    UPROPERTY(BlueprintAssignable)
    FGKWaitTargetDataDelegate Cancelled;

    UFUNCTION()
    void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle &Data, FGameplayTag ActivationTag);

    UFUNCTION()
    void OnTargetDataReplicatedCancelledCallback();

    UFUNCTION()
    void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle &Data);

    UFUNCTION()
    void OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle &Data);

    /** Uses specified target actor and waits for it to return valid data or to be canceled. */
    UFUNCTION(BlueprintCallable,
              meta     = (HidePin                  = "OwningAbility",
                      DefaultToSelf            = "OwningAbility",
                      BlueprintInternalUseOnly = "true",
                      HideSpawnParms           = "Instigator"),
              Category = "Ability|Tasks")
    static UGKAbilityTask_WaitForTargetData *WaitForTargetDataUsingActor(
            UGameplayAbility *                                OwningAbility,
            FName                                             TaskInstanceName,
            TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType,
            class AGKAbilityTarget_Actor *                    TargetActor);

    virtual void Activate() override;

    /** Called when the ability is asked to confirm from an outside node. What this means depends on the individual
     * task. By default, this does nothing other than ending if bEndTask is true. */
    virtual void ExternalConfirm(bool bEndTask) override;

    /** Called when the ability is asked to cancel from an outside node. What this means depends on the individual task.
     * By default, this does nothing other than ending the task. */
    virtual void ExternalCancel() override;

    protected:
    bool ShouldProduceTargetData() const;

    void InitializeTargetActor() const;

    void FinalizeTargetActor() const;

    void RegisterTargetDataCallbacks();

    virtual void OnDestroy(bool AbilityEnded) override;

    bool ShouldReplicateDataToServer() const;

    protected:
    class AGKAbilityTarget_Actor *TargetActor;

    TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType;

    FDelegateHandle OnTargetDataReplicatedCallbackDelegateHandle;
};
