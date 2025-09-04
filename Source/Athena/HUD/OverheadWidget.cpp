// Mike Hayes - All Rights Reserved


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole localRole = InPawn->GetLocalRole();
	FString role;
	switch (localRole)
	{
	case ENetRole::ROLE_Authority:
		role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		role = FString("None");
		break;
	}

	FString localRoleString = FString::Printf(TEXT("Local Role %s"), *role);
	SetDisplayText(localRoleString);
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	const APlayerState* PlayerState = InPawn->GetPlayerState();
    if(!PlayerState || !*PlayerState->GetPlayerName() && TotalTime < GetPlayerNameTimeout)
    {
        FTimerHandle GetPlayerStateTimer;
        FTimerDelegate TryAgainDelegate;
        TryAgainDelegate.BindUFunction(this, FName("ShowPlayerName"), InPawn);
        GetWorld()->GetTimerManager().SetTimer(GetPlayerStateTimer, TryAgainDelegate, GetPlayerNameInterval, false, 0.1f);
        TotalTime += GetPlayerNameInterval;
        return;
    }
    const FString PlayerName = InPawn->GetPlayerState()->GetPlayerName();
    SetDisplayText(PlayerName);
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
