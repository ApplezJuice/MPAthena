// Fill out your copyright notice in the Description page of Project Settings.


#include "UWorldListEntry.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "UWorldListItem.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.h"

void UWorldListEntry::MarkAsSelected(bool bSelected)
{
	if (bSelected) BORDER_Entry->SetBrushColor(HighlightColor);
    else BORDER_Entry->SetBrushColor(DefaultColor);
}

void UWorldListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (ListItemObject)
    {
        TObjectPtr<UWorldListItem> Item = Cast<UWorldListItem>(ListItemObject);
        if (Item)
        {
            const FOnlineSessionSearchResult* Result = Item->GetSearchResult();
            if (Result)
            {
                 TXT_Server->SetText(FText::FromString(*Result->Session.OwningUserName));
            }
            else
            {
                  // skipping Teststring code for discord limits - this can be removed anyway later
            }
        }
    }
}
