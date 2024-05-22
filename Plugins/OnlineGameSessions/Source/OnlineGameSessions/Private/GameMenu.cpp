// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMenu.h"

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
}
