// Fill out your copyright notice in the Description page of Project Settings.


#include "UWorldListItem.h"

void UWorldListItem::Initialize(const FOnlineSessionSearchResult* SessionSearchResult)
{
	SearchResult = SessionSearchResult;
	bIsSelected = false;
}
