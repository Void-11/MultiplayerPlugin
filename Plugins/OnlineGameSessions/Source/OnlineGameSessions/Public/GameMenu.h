// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

public:
	
	UFUNCTION(BlueprintCallable)
	void MenuSetup();

	virtual bool Initialize() override;

private:
	
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton{ nullptr };

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton{ nullptr };
	
	UFUNCTION()
	void OnHostButtonClicked();

	UFUNCTION()
	void OnJoinButtonClicked();

	UPROPERTY()
	UOnlineSessionsSubsystem* OnlineSessionsSubsystem{ nullptr };
	
};
