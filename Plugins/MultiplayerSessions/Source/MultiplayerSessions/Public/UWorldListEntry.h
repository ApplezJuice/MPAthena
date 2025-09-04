// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "UWorldListEntry.generated.h"

/**
 * 
 */

class UTextBlock;
class UBorder;

UCLASS()
class MULTIPLAYERSESSIONS_API UWorldListEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
    void MarkAsSelected(bool bSelected);

protected:

    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_Server;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_MatchType;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_NumPlayers;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> BORDER_Entry;

private:

    UPROPERTY(EditAnywhere)
    FLinearColor HighlightColor = FLinearColor(1.0f, 0.0f, 0.0f, 0.2f);

    UPROPERTY(EditAnywhere)
    FLinearColor DefaultColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.2f);
};
