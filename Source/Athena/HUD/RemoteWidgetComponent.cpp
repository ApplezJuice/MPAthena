// Mike Hayes - All Rights Reserved


#include "RemoteWidgetComponent.h"

void URemoteWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (auto* world = GetWorld())
	{
		OwnerPlayer = world->GetFirstLocalPlayerFromController();
	}
}
