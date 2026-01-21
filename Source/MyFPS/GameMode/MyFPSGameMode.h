// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyFPSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MYFPS_API AMyFPSGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void PlayerEliminated(class AMyFPSCharacter* ElimmedPlayer, class AMyFPSPlayerController* VictimController, AMyFPSPlayerController* AttackerController);
	virtual void RequestRespawn(AMyFPSCharacter* ElimmedPlayer, AMyFPSPlayerController* ElimmedController);
};
