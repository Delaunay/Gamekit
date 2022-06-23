// MIT License Copyright(c) 2020 Dan Kestranek
// Adapted for Gamekit by Pierre Delaunay

#include "Gamekit/Abilities/Blueprint/GKAsyncTaskAttributeChanged.h"

// Gamekit
#include "Gamekit/GKLog.h"
#include "Gamekit/Gamekit.h"

UGKAsyncTaskAttributeChanged *UGKAsyncTaskAttributeChanged::ListenForAttributeChange(
        UAbilitySystemComponent *AbilitySystemComponent,
        FGameplayAttribute       Attribute)
{
    UGKAsyncTaskAttributeChanged *WaitForAttributeChangedTask = NewObject<UGKAsyncTaskAttributeChanged>();
    WaitForAttributeChangedTask->ASC                          = AbilitySystemComponent;
    WaitForAttributeChangedTask->AttributeToListenFor         = Attribute;
    WaitForAttributeChangedTask->bDestroyed                   = false;

    if (!IsValid(AbilitySystemComponent) || !Attribute.IsValid())
    {
        WaitForAttributeChangedTask->RemoveFromRoot();
        return nullptr;
    }

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(
            WaitForAttributeChangedTask, &UGKAsyncTaskAttributeChanged::AttributeChanged);

    return WaitForAttributeChangedTask;
}

UGKAsyncTaskAttributeChanged *UGKAsyncTaskAttributeChanged::ListenForAttributesChange(
        UAbilitySystemComponent   *AbilitySystemComponent,
        TArray<FGameplayAttribute> Attributes)
{
    UGKAsyncTaskAttributeChanged *WaitForAttributeChangedTask = NewObject<UGKAsyncTaskAttributeChanged>();
    WaitForAttributeChangedTask->ASC                          = AbilitySystemComponent;
    WaitForAttributeChangedTask->AttributesToListenFor        = Attributes;
    WaitForAttributeChangedTask->bDestroyed                   = false;

    if (!IsValid(AbilitySystemComponent) || Attributes.Num() < 1)
    {
        WaitForAttributeChangedTask->RemoveFromRoot();
        return nullptr;
    }

    WaitForAttributeChangedTask->DelegateHandles.Reserve(Attributes.Num());
    for (FGameplayAttribute Attribute: Attributes)
    {
        WaitForAttributeChangedTask->DelegateHandles.Add(
                AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(
                        WaitForAttributeChangedTask, &UGKAsyncTaskAttributeChanged::AttributeChanged));
    }

    return WaitForAttributeChangedTask;
}

void UGKAsyncTaskAttributeChanged::EndTask()
{
    if (bDestroyed)
    {
        GK_ERROR(TEXT("Destroying object twice"));
        return;
    }

    SetReadyToDestroy();
}

void UGKAsyncTaskAttributeChanged::SetReadyToDestroy()
{
    if (IsValid(ASC))
    {
        ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);

        int i = 0;
        for (FGameplayAttribute Attribute: AttributesToListenFor)
        {
            ASC->GetGameplayAttributeValueChangeDelegate(Attribute).Remove(DelegateHandles[i]);
            i += 1;
        }
    }

    if (IsValid(this))
    {
        Super::SetReadyToDestroy();
    }

    bDestroyed = true;
}

void UGKAsyncTaskAttributeChanged::AttributeChanged(const FOnAttributeChangeData &Data)
{
    OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}