// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GameMenu.generated.h"

class UButton;
class UOnlineSessionsSubsystem;

/**
 * A base class for widgets interacting with the MultiplayerSessions system.
 */
UCLASS()
class ONLINEGAMESESSIONS_API UGameMenu : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeDestruct() override;
	
public:
	
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 InNumPublicConnections = 7,
				   const FString& InMatchType = TEXT("FreeForAll"),
				   const FString& LobbyPath = FString("/Game/ThirdPerson/Maps/GameLobby"));

	virtual bool Initialize() override;

protected:

	// Callbacks bound to delegates from MultiplayerOnCreateSessionComplete
	// NOTE: UFUNCTION added where passed to a dynamic delegate!

	/**
	 * @brief Called when session creation completes.
	 * @param bWasSuccessful True if session was successfully created, false
	 * otherwise.
	 */
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);

	/**
	 * @brief invoked when search  operation completes.
	 * @param SearchResults The results returned from find query
	 * @param bWasSuccessful True if operation was successful, false otherwise.
	 */
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);

	/**
	 * @brief Called when Join session attempt completes.
	 * @param Result Result of join attempt.
	 */
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	/**
	 * @brief Called when session destroy completes.
	 * @param bWasSuccessful True if session was successfully created, false
	 * otherwise.
	 */
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	/**
	 * @brief Called when start session completes.
	 * @param bWasSuccessful True if operation was successful, false otherwise.
	 */
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	
private:
	
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton{ nullptr };

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton{ nullptr };

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton{ nullptr };
	
	UFUNCTION()
	void OnHostButtonClicked();

	UFUNCTION()
	void OnJoinButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();
	
	UPROPERTY()
	UOnlineSessionsSubsystem* OnlineSessionsSubsystem{ nullptr };

	void MenuTearDown();

	int32 NumPublicConnections{ 7 };
	FString MatchType{ TEXT("FreeForAll") };
	FString PathToLobby{ TEXT("") };
};
