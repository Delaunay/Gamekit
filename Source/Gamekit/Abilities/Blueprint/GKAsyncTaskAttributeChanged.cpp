//  MIT License Copyright(c) 2020 Dan Kestranek

#include "Gamekit/Abilities/Blueprint/GKAsyncTaskAttributeChanged.h"

UGKAsyncTaskAttributeChanged *UGKAsyncTaskAttributeChanged::ListenForAttributeChange(
        UAbilitySystemComponent *AbilitySystemComponent,
        FGameplayAttribute       Attribute)
{
    UGKAsyncTaskAttributeChanged *WaitForAttributeChangedTask = NewObject<UGKAsyncTaskAttributeChanged>();
    WaitForAttributeChangedTask->ASC                          = AbilitySystemComponent;
    WaitForAttributeChangedTask->AttributeToListenFor         = Attribute;

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
        UAbilitySystemComponent *  AbilitySystemComponent,
        TArray<FGameplayAttribute> Attributes)
{
    UGKAsyncTaskAttributeChanged *WaitForAttributeChangedTask = NewObject<UGKAsyncTaskAttributeChanged>();
    WaitForAttributeChangedTask->ASC                          = AbilitySystemComponent;
    WaitForAttributeChangedTask->AttributesToListenFor        = Attributes;

    if (!IsValid(AbilitySystemComponent) || Attributes.Num() < 1)
    {
        WaitForAttributeChangedTask->RemoveFromRoot();
        return nullptr;
    }

    for (FGameplayAttribute Attribute: Attributes)
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(
                WaitForAttributeChangedTask, &UGKAsyncTaskAttributeChanged::AttributeChanged);
    }

    return WaitForAttributeChangedTask;
}

void UGKAsyncTaskAttributeChanged::EndTask()
{
    if (IsValid(ASC))
    {
        ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);

        for (FGameplayAttribute Attribute: AttributesToListenFor)
        {
            ASC->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
        }
    }

    SetReadyToDestroy();
    MarkPendingKill();
}

void UGKAsyncTaskAttributeChanged::AttributeChanged(const FOnAttributeChangeData &Data)
{
    OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}