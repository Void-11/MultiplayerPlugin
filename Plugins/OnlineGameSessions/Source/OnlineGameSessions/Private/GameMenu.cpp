// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMenu.h"
#include "Components/Button.h"
#include "OnlineSessionsSubsystem.h"
void UGameMenu::MenuSetup()
{
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

void UGameMenu::OnHostButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			17.f,
			FColor::Cyan,
			FString(TEXT("Host Button Clicked!")));
	}
	if (OnlineSessionsSubsystem)
	{
		HostButton->SetIsEnabled(false);
		OnlineSessionsSubsystem->CreateSession(4, FString("FreeForAll"),1);
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
