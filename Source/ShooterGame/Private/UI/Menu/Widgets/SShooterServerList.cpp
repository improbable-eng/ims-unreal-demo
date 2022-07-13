// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "SShooterServerList.h"
#include "SHeaderRow.h"
#include "ShooterStyle.h"
#include "ShooterGameLoadingScreen.h"
#include "ShooterGameInstance.h"
#include "Online/ShooterGameSession.h"

#define LOCTEXT_NAMESPACE "ShooterGame.HUD.Menu"

void SShooterServerList::Construct(const FArguments& InArgs)
{
	PlayerOwner = InArgs._PlayerOwner;
	OwnerWidget = InArgs._OwnerWidget;
	MapFilterName = "Any";
	bSearchingForServers = false;
	StatusText = FText::GetEmpty();
	BoxWidth = 125;
	LastSearchTime = 0.0f;
	
#if PLATFORM_SWITCH
	MinTimeBetweenSearches = 6.0;
#else
	MinTimeBetweenSearches = 0.0;
#endif

	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)  
			.WidthOverride(800)
			.HeightOverride(300)
			[
				SAssignNew(ServerListWidget, SListView<TSharedPtr<FServerEntry>>)
				.ItemHeight(20)
				.ListItemsSource(&ServerList)
				.SelectionMode(ESelectionMode::Single)
				.OnGenerateRow(this, &SShooterServerList::MakeListViewWidget)
				.OnSelectionChanged(this, &SShooterServerList::EntrySelectionChanged)
				.OnMouseButtonDoubleClick(this,&SShooterServerList::OnListItemDoubleClicked)
				.HeaderRow(
					SNew(SHeaderRow)
					+ SHeaderRow::Column("Address").FixedWidth(BoxWidth * 2).DefaultLabel(NSLOCTEXT("Address", "AddressColumn", "Address"))
					+ SHeaderRow::Column("GamePhase").DefaultLabel(NSLOCTEXT("GamePhase", "GamePhaseColumn", "Game Phase"))
					+ SHeaderRow::Column("MapName").DefaultLabel(NSLOCTEXT("MapName", "MapNameColumn", "Map Name"))
					+ SHeaderRow::Column("PlayerCount").DefaultLabel(NSLOCTEXT("PlayerCount", "PlayerCountColumn", "Player Count")))
			]
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SRichTextBlock)
				.Text(this, &SShooterServerList::GetBottomText)
				.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuServerListTextStyle")
				.DecoratorStyleSet(&FShooterStyle::Get())
				+ SRichTextBlock::ImageDecorator()
			]
		]
		
	];
}

/** 
 * Get the current game session
 */
AShooterGameSession* SShooterServerList::GetGameSession() const
{
	UShooterGameInstance* const GI = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
	return GI ? GI->GetGameSession() : nullptr;
}

/** Updates current search status */
void SShooterServerList::UpdateSearchStatus()
{
	check(bSearchingForServers); // should not be called otherwise

	bool bFinishSearch = true;
	AShooterGameSession* ShooterSession = GetGameSession();
	if (ShooterSession)
	{
		SearchState SearchState = ShooterSession->GetSearchSessionsStatus();

		UE_LOG(LogOnlineGame, Log, TEXT("ShooterSession->GetSearchResultStatus: %s"), *StateToString(SearchState));

		switch(SearchState)
		{
			case SearchState::InProgress:
				StatusText = LOCTEXT("Searching","SEARCHING...");
				bFinishSearch = false;
				break;

			case SearchState::Done:
				{
					ServerList.Empty();
					const TArray<Session>& SearchResults = ShooterSession->GetSearchResults();

					if (SearchResults.Num() == 0)
					{
						StatusText = LOCTEXT("ServersRefresh", "PRESS SPACE TO REFRESH SERVER LIST");
					}

					for (int32 IdxResult = 0; IdxResult < SearchResults.Num(); ++IdxResult)
					{
						TSharedPtr<FServerEntry> NewServerEntry = MakeShareable(new FServerEntry());

						const Session& Result = SearchResults[IdxResult];

						NewServerEntry->GamePhase = Result.GetGamePhase();
						NewServerEntry->SessionAddress = Result.GetSessionAddress();
						NewServerEntry->PlayerCount = Result.GetPlayerCount();
						NewServerEntry->MapName = Result.GetMapName();
						NewServerEntry->SearchResultsIndex = IdxResult;

						ServerList.Add(NewServerEntry);
					}
				}
				break;

			case SearchState::Failed:
				// intended fall-through
			case SearchState::NotStarted:
				StatusText = FText::GetEmpty();
				// intended fall-through
			default:
				break;
		}
	}

	if (bFinishSearch)
	{		
		OnServerSearchFinished();
	}
}


FText SShooterServerList::GetBottomText() const
{
	 return StatusText;
}

/**
 * Ticks this widget.  Override in derived classes, but always call the parent implementation.
 *
 * @param  InCurrentTime  Current absolute real time
 * @param  InDeltaTime  Real time passed since last tick
 */
void SShooterServerList::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
	SCompoundWidget::Tick( AllottedGeometry, InCurrentTime, InDeltaTime );

	if ( bSearchingForServers )
	{
		UpdateSearchStatus();
	}
}

/** Starts searching for servers */
void SShooterServerList::BeginServerSearch()
{
	double CurrentTime = FApp::GetCurrentTime();
	if (CurrentTime - LastSearchTime < MinTimeBetweenSearches)
	{
		OnServerSearchFinished();
	}
	else
	{
		bSearchingForServers = true;
		ServerList.Empty();
		LastSearchTime = CurrentTime;

		UShooterGameInstance* const GI = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
		if (GI)
		{
			GI->FindSessions(PlayerOwner.Get());
		}
	}
}

/** Called when server search is finished */
void SShooterServerList::OnServerSearchFinished()
{
	bSearchingForServers = false;

	UpdateServerList();
}

void SShooterServerList::UpdateServerList()
{
	int32 SelectedItemIndex = ServerList.IndexOfByKey(SelectedItem);

	ServerListWidget->RequestListRefresh();
	if (ServerList.Num() > 0)
	{
		ServerListWidget->UpdateSelectionSet();
		ServerListWidget->SetSelection(ServerList[SelectedItemIndex > -1 ? SelectedItemIndex : 0],ESelectInfo::OnNavigation);
	}

}

void SShooterServerList::ConnectToServer()
{
	if (bSearchingForServers)
	{
		// unsafe
		return;
	}

	if (SelectedItem.IsValid())
	{
		int ServerToJoin = SelectedItem->SearchResultsIndex;

		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->RemoveAllViewportWidgets();
		}
		
		UShooterGameInstance* const GI = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
		if (GI)
		{
			GI->JoinSession(PlayerOwner.Get(), ServerToJoin);
		}
	}
}

void SShooterServerList::OnFocusLost( const FFocusEvent& InFocusEvent )
{
	if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
	{
		FSlateApplication::Get().SetKeyboardFocus(SharedThis( this ));
	}
}

FReply SShooterServerList::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled().SetUserFocus(ServerListWidget.ToSharedRef(), EFocusCause::SetDirectly).SetUserFocus(SharedThis(this), EFocusCause::SetDirectly, true);
}

void SShooterServerList::EntrySelectionChanged(TSharedPtr<FServerEntry> InItem, ESelectInfo::Type SelectInfo)
{
	SelectedItem = InItem;
}

void SShooterServerList::OnListItemDoubleClicked(TSharedPtr<FServerEntry> InItem)
{
	SelectedItem = InItem;
	ConnectToServer();
	FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
}

void SShooterServerList::MoveSelection(int32 MoveBy)
{
	int32 SelectedItemIndex = ServerList.IndexOfByKey(SelectedItem);

	if (SelectedItemIndex+MoveBy > -1 && SelectedItemIndex+MoveBy < ServerList.Num())
	{
		ServerListWidget->SetSelection(ServerList[SelectedItemIndex+MoveBy]);
	}
}

FReply SShooterServerList::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) 
{
	if (bSearchingForServers) // lock input
	{
		return FReply::Handled();
	}

	FReply Result = FReply::Unhandled();
	const FKey Key = InKeyEvent.GetKey();
	
	if (Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
	{
		MoveSelection(-1);
		Result = FReply::Handled();
	}
	else if (Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
	{
		MoveSelection(1);
		Result = FReply::Handled();
	}
	else if (Key == EKeys::Enter || Key == EKeys::Virtual_Accept)
	{
		ConnectToServer();
		Result = FReply::Handled();
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
	}
	//hit space bar to search for servers again / refresh the list, only when not searching already
	else if (Key == EKeys::SpaceBar || Key == EKeys::Gamepad_FaceButton_Left)
	{
		BeginServerSearch();
	}
	return Result;
}

TSharedRef<ITableRow> SShooterServerList::MakeListViewWidget(TSharedPtr<FServerEntry> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	class SServerEntryWidget : public SMultiColumnTableRow< TSharedPtr<FServerEntry> >
	{
	public:
		SLATE_BEGIN_ARGS(SServerEntryWidget){}
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, TSharedPtr<FServerEntry> InItem)
		{
			Item = InItem;
			SMultiColumnTableRow< TSharedPtr<FServerEntry> >::Construct(FSuperRowType::FArguments(), InOwnerTable);
		}

		TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName)
		{
			FText ItemText = FText::GetEmpty();

			if (ColumnName == "Address")
			{
				ItemText = FText::FromString(Item->SessionAddress);
			}
			else if (ColumnName == "GamePhase")
			{
				ItemText = FText::FromString(Item->GamePhase);
			}
			else if (ColumnName == "MapName")
			{
				ItemText = FText::FromString(Item->MapName);
			}
			else if (ColumnName == "PlayerCount")
			{
				ItemText = FText::FromString(Item->PlayerCount);
			}
			return SNew(STextBlock)
				.Text(ItemText)
				.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuServerListTextStyle");
		}
		TSharedPtr<FServerEntry> Item;
	};
	return SNew(SServerEntryWidget, OwnerTable, Item);
}

#undef LOCTEXT_NAMESPACE