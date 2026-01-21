// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFPSGameMode.h"
#include "MyFPS/Character/MyFPSCharacter.h"
#include "MyFPS/MyFPSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void AMyFPSGameMode::PlayerEliminated(AMyFPSCharacter* ElimmedPlayer, AMyFPSPlayerController* VictimController, AMyFPSPlayerController* AttackerController)
{
	if(ElimmedPlayer)
	{
		ElimmedPlayer->Elim();
	}
}

void AMyFPSGameMode::RequestRespawn(AMyFPSCharacter* ElimmedPlayer, AMyFPSPlayerController* ElimmedController)
{
	if (ElimmedPlayer)
	{
		ElimmedPlayer->Reset();
		ElimmedPlayer->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*>PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
