// Fill out your copyright notice in the Description page of Project Settings.


#include "UWordList.h"
#include "UWorldListEntry.h"
#include "UWorldListItem.h"

void UWordList::OnSelectionChangedInternal(UObject* FirstSelectedItem)
{
	Super::OnSelectionChangedInternal(FirstSelectedItem);
    TObjectPtr<UWorldListItem> Item = Cast<UWorldListItem>(FirstSelectedItem);

    if (Item)
    {
        Item->Select(true);
        if (LastSelectedEntry) LastSelectedEntry->Select(false);
        LastSelectedEntry = Item;

        TArray<TObjectPtr<UUserWidget>> ShownWidgets;
        ShownWidgets = GetDisplayedEntryWidgets();
        for (UUserWidget* Widget : ShownWidgets)
        {
            TObjectPtr<UWorldListEntry> MatchEntryWidget = Cast<UWorldListEntry>(Widget);
            if (MatchEntryWidget)
            {
                TObjectPtr<UWorldListItem> ItemData = Cast<UWorldListItem>(MatchEntryWidget->GetListItem());
                if (ItemData) MatchEntryWidget->MarkAsSelected(ItemData->IsSelected());
            }
        }
    }
    OnWorldSelectionChanged.Broadcast(Item, Item != nullptr);
}

void UWordList::OnItemDoubleClickedInternal(UObject* Item)
{
    TObjectPtr<UWorldListItem> DoubleClickedItem = Cast<UWorldListItem>(Item);
    if (DoubleClickedItem) OnWorldDoubleClicked.Broadcast(DoubleClickedItem);
}
