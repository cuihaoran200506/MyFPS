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

		if (NumberOfPlayers == 2)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				bUseSeamlessTravel = true;
				World->ServerTravel(FString("/Game/FirstPerson/Maps/BlasterMap?listen"));
			}
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
	if (PlayerState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		GEngine->AddOnScreenDebugMessage(
			1,
			600.f,
			FColor::Green,
			FString::Printf(TEXT("Total Players: %d"), NumberOfPlayers-1)
		);
		FString PlayerName = PlayerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage(
			2,
			60.f,
			FColor::Cyan,
			FString::Printf(TEXT("%s Exited The Game"), *PlayerName)
		);
	}
}
