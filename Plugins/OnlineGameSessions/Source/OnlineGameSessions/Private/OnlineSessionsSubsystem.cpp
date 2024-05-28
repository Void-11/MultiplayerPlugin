// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UOnlineSessionsSubsystem::UOnlineSessionsSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

void UOnlineSessionsSubsystem::CreateSession(const int32 NumPublicConnections, const FString& MatchType)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}

	// Store the delegate in a FDelegateHandle so we can later remove it from the delegate list
	CreateSessionCompleteHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);

		// Broadcast our own custom delegate
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

void UOnlineSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
}

void UOnlineSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
}

void UOnlineSessionsSubsystem::DestroySession()
{
}

void UOnlineSessionsSubsystem::StartSession()
{
}

void UOnlineSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
	}

	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UOnlineSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
}

void UOnlineSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
}

void UOnlineSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void UOnlineSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}
