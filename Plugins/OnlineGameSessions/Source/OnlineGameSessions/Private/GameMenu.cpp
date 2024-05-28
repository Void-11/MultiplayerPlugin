// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMenu.h"
#include "Components/Button.h"
#include "OnlineSessionsSubsystem.h"

void UGameMenu::MenuSetup(const int32 InNumPublicConnections, const FString& InMatchType, const FString& InPathToLobby)
{
	NumPublicConnections = InNumPublicConnections;
	MatchType = InMatchType;
	
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);;

	if (const auto World = GetWorld())
	{
		if (const auto PlayerController = World->GetFirstPlayerController())
		{
			// Data structure used to set up an input mode that allows only the UI to
			// respond to user input
			FInputModeUIOnly InputModeData;

			InputModeData.SetWidgetToFocus(TakeWidget());

			// Don't lock mouse cursor to the viewport so mouse can move off-screen
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PlayerController->SetInputMode(InputModeData);

			// Make the mouse visible so user can see what they are doing
			PlayerController->SetShowMouseCursor(true);
		}
	}

	if (const auto GameInstance = GetGameInstance())
	{
		OnlineSessionsSubsystem = GameInstance->GetSubsystem<UOnlineSessionsSubsystem>();
	}

	if (OnlineSessionsSubsystem)
	{
		OnlineSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UGameMenu::OnCreateSession);
		OnlineSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &UGameMenu::OnFindSessions);
		OnlineSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &UGameMenu::OnJoinSession);
		OnlineSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UGameMenu::OnDestroySession);
		OnlineSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &UGameMenu::OnStartSession);
	}
}

void UGameMenu::MenuTearDown()
{
	// The "inverse" of MenuSetup above

	RemoveFromParent();

	if (const auto World = GetWorld())
	{
		if (const auto PlayerController = World->GetFirstPlayerController())
		{
			// Data structure used to set up an input mode that allows only the Game to
			// respond to user input
			const FInputModeGameOnly InputModeData;

			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

bool UGameMenu::Initialize()
{
	if (Super::Initialize())
	{
		if (HostButton)
		{
			HostButton->OnClicked.AddDynamic(this, &UGameMenu::OnHostButtonClicked);
		}
		if (JoinButton)
		{
			JoinButton->OnClicked.AddDynamic(this, &UGameMenu::OnJoinButtonClicked);
		}
		
		return true;
	}
	else
	{
		return false;
	}
}

void UGameMenu::NativeDestruct()
{
	// Invoked when the underlying ui widget disappears which means we can tear
	// down menu infrastructure and re-enable user input to the game

	MenuTearDown();

	Super::NativeDestruct();
}

void UGameMenu::OnHostButtonClicked()
{
	if (OnlineSessionsSubsystem)
	{
		HostButton->SetIsEnabled(false);
		OnlineSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
		
	}
}

void UGameMenu::OnJoinButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			17.f,
			FColor::Magenta,
			FString(TEXT("Join Button Clicked!")));
	}
	if (OnlineSessionsSubsystem)
	{
		JoinButton->SetIsEnabled(false);
		// We set a very high session count as we are using the DevId for game and
		// lots of other devs will be adding sessions
		OnlineSessionsSubsystem->FindSessions(10000);
	}
}

void UGameMenu::OnCreateSession(bool bWasSuccessful)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 17.f, FColor::Yellow, FString(TEXT("UMenu::OnCreateSessionComplete ")));
	}
	HostButton->SetIsEnabled(true);
	
	if (bWasSuccessful)
	{
		if (const auto World = GetWorld())
		{
			World->ServerTravel(FString("/Game/ThirdPerson/Maps/GameLobby?listen"));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 17.f, FColor::Red, FString(TEXT("Failed to create session")));
		}
	}
}

void UGameMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful)
{
}

void UGameMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
}

void UGameMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UGameMenu::OnStartSession(bool bWasSuccessful)
{
}
