// Mike Hayes - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class ATHENA_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Associates C++ var with text block in the widget blueprint
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	void SetDisplayText(FString TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerName(APawn* InPawn);

protected:
	// called when transitioned from a level etc.
	virtual void NativeDestruct() override;

private:
	UPROPERTY(EditAnywhere, Category="Overhead Widget Properties", meta=(AllowPrivateAccess=true, Units="Seconds"))
    float GetPlayerNameTimeout = 30.f;
    UPROPERTY(EditAnywhere, Category="Overhead Widget Properties", meta=(AllowPrivateAccess=true, Units="Seconds"))
    float GetPlayerNameInterval = 0.1f;
    float TotalTime = -0.1f;
};
