// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Delegates/Delegate.h"
#include "UObject/Interface.h"

// Generated
#include "GKNetworkedEvent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGKEventGate);


USTRUCT()
struct GAMEKIT_API FGKEvent {
    GENERATED_USTRUCT_BODY()

    FGKEvent(FName Name = NAME_None, int Count = 0):
        Name(Name), Count(Count)
    {}

    FName Name;
    int Count;
};

// Waits for a list of event to be received,
// before broadcasting its own event
USTRUCT()
struct GAMEKIT_API FGKEventGatherer {
    GENERATED_USTRUCT_BODY()

public:
    FGKEventGatherer():
        Received(0)
    {
    }

    void RecordEvent(FName EventName) {
        bool AllReceived = true;

        for (FGKEvent& Event: RequiredEvents){
            if (Event.Name == EventName){
                if (Event.Count == 0)
                    Received += 1;

                Event.Count += 1;
            }
            AllReceived &= Event.Count >= 1;
        }

        if (AllReceived) {
            OnAllEventGathered.Broadcast();
        }
    }

    bool IsRegistered(UObject* Obj, FName Func){
        return OnAllEventGathered.Contains(Obj, Func);
    }

    int GetTotalReceived() {
        return Received;
    }

    int GetMissing() {
        return Received - RequiredEvents.Num();
    }

    void AddRequiredEvent(FName Name) {
        for (FGKEvent& Event : RequiredEvents) {
            if (Event.Name == Name)
                return;
        }

        RequiredEvents.Emplace(Name, 0);
    }

    FGKEventGate GetDelegate() {
        return OnAllEventGathered;
    }

private:
    int          Received;
    FGKEventGate OnAllEventGathered;
    TArray<FGKEvent> RequiredEvents;
};



UINTERFACE()
class GAMEKIT_API UActorReadyInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};


class GAMEKIT_API IActorReadyInterface
{
	GENERATED_IINTERFACE_BODY()

    virtual void RecordEvent(FName Name){
        GetReadyEventGather().RecordEvent(Name);
    }

    virtual void AddRequiredEvent(FName Name) {
        GetReadyEventGather().AddRequiredEvent(Name);
    }

    virtual FGKEventGatherer& GetReadyEventGather() = 0;
};
