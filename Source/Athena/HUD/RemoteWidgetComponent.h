// Mike Hayes - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "RemoteWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class ATHENA_API URemoteWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
};
