// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ONLINEGAMESESSIONS_API UOnlineSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	UOnlineSessionsSubsystem();

	// Called by Menu class.
	UFUNCTION()
	void CreateSession(int32 NumPublicConnections, FString MatchType, int32 UniqueId);

	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	
	UFUNCTION()
	void DestroySession();
	UFUNCTION()
	void StartSession();

protected:
	
	// Internal callbacks for the delegates added to the Online Session 
	// Interface list. These are called by the MultiplayerSessionsMenu and
	// do not need to be called outside this class.
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	
	// Online Session Interface delegate and delegate handle list.
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteHandle;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteHandle;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteHandle;
};
