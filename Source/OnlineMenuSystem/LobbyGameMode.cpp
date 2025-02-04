// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				600.f,
				FColor::Red,
				FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers)
			);

			if (APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>())
			{
				FString PlayerName = PlayerState->GetPlayerName();
				GEngine->AddOnScreenDebugMessage(
					2,
					60.f,
					FColor::Purple,
					FString::Printf(TEXT("%s has joined the game!"), *PlayerName)
				);
			}
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>())
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		GEngine->AddOnScreenDebugMessage(
			1,
			600.f,
			FColor::Orange,
			FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers - 1)
		);

		FString PlayerName = PlayerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage(
			2,
			60.f,
			FColor::Magenta,
			FString::Printf(TEXT("%s has exited the game!"), *PlayerName)
		);
	}
}
