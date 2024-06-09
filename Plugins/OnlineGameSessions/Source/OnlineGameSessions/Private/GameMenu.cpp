// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMenu.h"
#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "OnlineSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameMenu::MenuSetup(const int32 InNumPublicConnections, const FString& InMatchType, const FString& LobbyPath)
{
	NumPublicConnections = InNumPublicConnections;
	MatchType = InMatchType;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	
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
		if (QuitButton)
		{
			QuitButton->OnClicked.AddDynamic(this, &UGameMenu::OnQuitButtonClicked);
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

void UGameMenu::OnCreateSession(bool bWasSuccessful)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 17.f, FColor::Yellow, FString(TEXT("UMenu::OnCreateSessionComplete ")));
	}
	
	if (bWasSuccessful)
	{
		if (const auto World = GetWorld())
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 17.f, FColor::Red, FString(TEXT("Failed to create session")));
		}
		HostButton->SetIsEnabled(true);
	}
}

void UGameMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,
										 17.f,
										 FColor::Black,
										 FString::Printf(TEXT("UMenu::OnFindSessionsComplete - "
															  "bWasSuccessful=%d SearchResults.Len=%d"),
														 bWasSuccessful,
														 SearchResults.Num()));
	}
	if (OnlineSessionsSubsystem)
	{
		for (const auto Result : SearchResults)
		{
			// Look for a result that matches our desired MatchType
			if (FString ResultMatchType;
				Result.Session.SessionSettings.Get(FName("MatchType"), ResultMatchType) && ResultMatchType == MatchType)
			{
				OnlineSessionsSubsystem->JoinSession(Result);
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(
						-1,
						17.f,
						FColor::Yellow,
						FString(TEXT("UMenu::OnFindSessionsComplete - Found matching session")));
				}
				return;
			}
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
											 17.f,
											 FColor::Green,
											 FString(TEXT("UMenu::OnFindSessionsComplete - Failed to get subsystem")));
		}
		JoinButton->SetIsEnabled(true);
	}
	// This will capture all the error scenarios, success will be handled in
	// OnJoinSession
	
}

void UGameMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	   if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 17.f, FColor::Red, FString(TEXT("UMenu::OnJoinSessionComplete ")));
    }
    if (const auto Subsystem = Online::GetSubsystem(GetWorld()))
    {
        if (const auto SessionInterface = Subsystem->GetSessionInterface(); SessionInterface.IsValid())
        {
            if (FString Address; SessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
            {
                if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
                {
                    PlayerController->ClientTravel(Address, TRAVEL_Absolute);
                    JoinButton->SetIsEnabled(true);
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(
                            -1,
                            17.f,
                            FColor::Yellow,
                            FString(TEXT("UMenu::OnJoinSessionComplete - Client Travel Complete")));
                    }
                }
                else
                {
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1,
                                                         17.f,
                                                         FColor::Blue,
                                                         FString(TEXT("UMenu::OnJoinSessionComplete - No local player "
                                                                      "controller")));
                    }
                }
            }
            else
            {
                // TODO: Error here?
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1,
                                                     17.f,
                                                     FColor::Yellow,
                                                     FString(TEXT("UMenu::OnJoinSessionComplete - No get Address")));
                }
            }
        }
        else
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(
                    -1,
                    17.f,
                    FColor::Orange,
                    FString(TEXT("UMenu::OnJoinSessionComplete - Invalid Session INterface")));
            }
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1,
                                             17.f,
                                             FColor::Blue,
                                             FString(TEXT("UMenu::OnJoinSessionComplete - Failed to get subsystem")));
        }
    }
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

void UGameMenu::OnQuitButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 17.f, FColor::Green, FString(TEXT("Quit Button Clicked!")));
	}
	if (const auto World = GetWorld())
	{
		if (const auto PlayerController = World->GetFirstPlayerController())
		{

			UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
		}
	}
}

void UGameMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UGameMenu::OnStartSession(bool bWasSuccessful)
{
}
