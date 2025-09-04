// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "UWordList.generated.h"

class UWorldListItem;
class UWorldListEntry;

DECLARE_MULTICAST_DELEGATE_TwoParams(FWorldSelectionChanged, UWorldListItem*, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FWorldDoubleClicked, UWorldListItem*);

UCLASS(meta = (EntryInterface = UserObjectListEntry, EntryClass = UWorldListEntry))
class MULTIPLAYERSESSIONS_API UWordList : public UListView
{
	GENERATED_BODY()
	
public:
	FWorldSelectionChanged OnWorldSelectionChanged;
	FWorldDoubleClicked OnWorldDoubleClicked;

protected:
	virtual void OnSelectionChangedInternal(UObject* FirstSelectedItem) override;
	virtual void OnItemDoubleClickedInternal(UObject* Item) override;

private:
	UPROPERTY()
	TObjectPtr<UWorldListItem> LastSelectedEntry = nullptr;
};
