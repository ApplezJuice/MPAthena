// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "UWorldListItem.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeUIOnly inputModeData;
			inputModeData.SetWidgetToFocus(TakeWidget());
			inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance)
	{
		MultiplayerSessionsSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	// Custom Callback binds
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSessionCallback);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessionsCallback);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSessionCallback);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySessionCallback);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSessionCallback);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	// Bind callbacks to the buttons
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
		JoinButton->SetIsEnabled(false);
	}
	if (FindSessionsButton)
	{
		FindSessionsButton->OnClicked.AddDynamic(this, &ThisClass::FindSessionsButtonClicked);
	}

	return true;
}

void UMenu::NativeConstruct()
{
	if (LV_Worlds)
    {
		LV_Worlds->OnWorldSelectionChanged.AddUObject(this, &ThisClass::OnWorldSelectionChanged);
        LV_Worlds->OnWorldDoubleClicked.AddUObject(this, &ThisClass::OnWorldDoubleClicked);
    }
	Super::NativeConstruct();
}

void UMenu::NativeDestruct()
{
	MenuTeardown();
	Super::NativeDestruct();
}

void UMenu::OnCreateSessionCallback(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Green,
				FString(TEXT("Session Created Successfully!"))
			);
		}
		UWorld* world = GetWorld();
		if (world)
		{
			world->ServerTravel(PathToLobby);
		}
	}
	else 
	{
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessionsCallback(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}

	LV_Worlds->ClearListItems();

	for (int32 i = 0; i < SessionResults.Num(); i++)
    {
		FString settingsValue;
		SessionResults[i].Session.SessionSettings.Get(FName("MatchType"), settingsValue);
		if (settingsValue == MatchType)
		{
			UWorldListItem* Item = NewObject<UWorldListItem>(this);
			Item->Initialize(&SessionResults[i]);
			LV_Worlds->AddItem(Item);
		}
    }

	//for (FOnlineSessionSearchResult result : SessionResults)
	//{
	//	FString settingsValue;
	//	result.Session.SessionSettings.Get(FName("MatchType"), settingsValue);
	//	// Steam complains about bUseLobbiesIfAvailable and bUsesPresence must match
	//	// so changing these in the results, which should NOT be needed!
	//	// Joining fails otherwise, though, so doing it for now.
	//	result.Session.SessionSettings.bUseLobbiesIfAvailable = true;
	//	result.Session.SessionSettings.bUsesPresence = true;

	//	if (settingsValue == MatchType)
	//	{
	//		// Valid search result
	//		//MultiplayerSessionsSubsystem->JoinSession(result);
	//		return;
	//	}
	//}
}

void UMenu::OnJoinSessionCallback(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();
	if (subsystem)
	{
		IOnlineSessionPtr sessionInterface = subsystem->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			FString address;
			sessionInterface->GetResolvedConnectString(NAME_GameSession, address);

			APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (playerController)
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(
						-1,
						15.f,
						FColor::Green,
						FString::Printf(TEXT("Address: %s"), *address)
					);
				}
				playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnWorldSelectionChanged(UWorldListItem* world, bool bIsSlected)
{
	SelectedWorld = world;
	if (!JoinButton->bIsEnabled)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnWorldDoubleClicked(UWorldListItem* world)
{
	if (MultiplayerSessionsSubsystem)
	{
		FOnlineSessionSearchResult result = *world->GetSearchResult();
		result.Session.SessionSettings.bUseLobbiesIfAvailable = true;
		result.Session.SessionSettings.bUsesPresence = true;
		MultiplayerSessionsSubsystem->JoinSession(result);
	}
}

void UMenu::OnDestroySessionCallback(bool bWasSuccessful)
{
}

void UMenu::OnStartSessionCallback(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		if (SelectedWorld)
		{
			FOnlineSessionSearchResult result = *SelectedWorld->GetSearchResult();
			result.Session.SessionSettings.bUseLobbiesIfAvailable = true;
			result.Session.SessionSettings.bUsesPresence = true;
			MultiplayerSessionsSubsystem->JoinSession(result);
		}
	}
}

void UMenu::FindSessionsButtonClicked()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTeardown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeGameOnly inputModeData;
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(false);
		}
	}
}
