// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "UWorldListItem.generated.h"

class FOnlineSessionSearchResult;

// This class does not need to be modified.
UCLASS()
class MULTIPLAYERSESSIONS_API UWorldListItem : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(const FOnlineSessionSearchResult* SessionSearchResult);
	FString TestString = FString("none");

private:
	const FOnlineSessionSearchResult* SearchResult;
	bool bIsSelected = false;

public:
	FORCEINLINE const FOnlineSessionSearchResult* GetSearchResult() const { return SearchResult; }
	FORCEINLINE const bool IsSelected() const { return bIsSelected; }
	FORCEINLINE void Select(bool bSelect) { bIsSelected = bSelect; }
};
