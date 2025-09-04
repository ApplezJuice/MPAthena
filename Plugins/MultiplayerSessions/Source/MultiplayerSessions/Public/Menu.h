// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Interfaces/OnlineSessionInterface.h>
#include "UWordList.h"
#include "UWorldListEntry.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 10, FString TypeOfMatch = FString(TEXT("Default")), FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

protected:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	//
	// Callbacks for the custom delegates on MultiplayerSessionsSubsystem
	//
	UFUNCTION()
	void OnCreateSessionCallback(bool bWasSuccessful);
	UFUNCTION()
	void OnDestroySessionCallback(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSessionCallback(bool bWasSuccessful);

	void OnFindSessionsCallback(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSessionCallback(EOnJoinSessionCompleteResult::Type Result);

	void OnWorldSelectionChanged(UWorldListItem* world, bool bIsSlected);
	void OnWorldDoubleClicked(UWorldListItem* world);

private:
	// List of available online sessions
	UPROPERTY(meta = (BindWidget), meta = (EntryClass = UWorldListEntry))
	UWordList* LV_Worlds;

	// Must match the blueprint button name
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* FindSessionsButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	UFUNCTION()
	void FindSessionsButtonClicked();

	void MenuTeardown();

	// The subsystem designed to handle all online session functionality
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{10};
	FString MatchType{ TEXT("Default") };
	FString PathToLobby{ TEXT("") };
	UWorldListItem* SelectedWorld;
};
