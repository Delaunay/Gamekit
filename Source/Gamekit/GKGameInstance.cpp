// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "GKGameInstance.h"

#include "GKAssetManager.h"
#include "GKSaveGame.h"
#include "Items/GKItem.h"

#include "Kismet/GameplayStatics.h"

UGKGameInstanceBase::UGKGameInstanceBase(): SaveSlot(TEXT("SaveGame")), SaveUserIndex(0) {}

UGKSaveGame *UGKGameInstanceBase::GetCurrentSaveGame() { return CurrentSaveGame; }

void UGKGameInstanceBase::SetSavingEnabled(bool bEnabled) { bSavingEnabled = bEnabled; }

bool UGKGameInstanceBase::LoadOrCreateSaveGame()
{
    UGKSaveGame *LoadedSave = nullptr;

    if (UGameplayStatics::DoesSaveGameExist(SaveSlot, SaveUserIndex) && bSavingEnabled)
    {
        LoadedSave = Cast<UGKSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlot, SaveUserIndex));
    }

    return HandleSaveGameLoaded(LoadedSave);
}

bool UGKGameInstanceBase::HandleSaveGameLoaded(USaveGame *SaveGameObject)
{
    bool bLoaded = false;

    if (!bSavingEnabled)
    {
        // If saving is disabled, ignore passed in object
        SaveGameObject = nullptr;
    }

    // Replace current save, old object will GC out
    CurrentSaveGame = Cast<UGKSaveGame>(SaveGameObject);

    if (CurrentSaveGame)
    {
        bLoaded = true;
    }
    else
    {
        // This creates it on demand
        CurrentSaveGame = Cast<UGKSaveGame>(UGameplayStatics::CreateSaveGameObject(UGKSaveGame::StaticClass()));
    }

    OnSaveGameLoaded.Broadcast(CurrentSaveGame);
    OnSaveGameLoadedNative.Broadcast(CurrentSaveGame);

    return bLoaded;
}

void UGKGameInstanceBase::GetSaveSlotInfo(FString &SlotName, int32 &UserIndex) const
{
    SlotName  = SaveSlot;
    UserIndex = SaveUserIndex;
}

bool UGKGameInstanceBase::WriteSaveGame()
{
    if (bSavingEnabled)
    {
        if (bCurrentlySaving)
        {
            // Schedule another save to happen after current one finishes. We only queue one save
            bPendingSaveRequested = true;
            return true;
        }

        // Indicate that we're currently doing an async save
        bCurrentlySaving = true;

        // This goes off in the background
        UGameplayStatics::AsyncSaveGameToSlot(
                GetCurrentSaveGame(),
                SaveSlot,
                SaveUserIndex,
                FAsyncSaveGameToSlotDelegate::CreateUObject(this, &UGKGameInstanceBase::HandleAsyncSave));
        return true;
    }
    return false;
}

void UGKGameInstanceBase::ResetSaveGame()
{
    // Call handle function with no loaded save, this will reset the data
    HandleSaveGameLoaded(nullptr);
}

void UGKGameInstanceBase::HandleAsyncSave(const FString &SlotName, const int32 UserIndex, bool bSuccess)
{
    ensure(bCurrentlySaving);
    bCurrentlySaving = false;

    if (bPendingSaveRequested)
    {
        // Start another save as we got a request while saving
        bPendingSaveRequested = false;
        WriteSaveGame();
    }
}
