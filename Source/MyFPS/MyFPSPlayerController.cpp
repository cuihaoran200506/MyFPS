// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyFPSPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "MyFPSCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "MyFPS.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "MyFPS/HUD/MyFPSHUD.h"
#include "MyFPS/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

AMyFPSPlayerController::AMyFPSPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AMyFPSCameraManager::StaticClass();
}

void AMyFPSPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	MyFPSHUD = MyFPSHUD == nullptr ? Cast<AMyFPSHUD>(GetHUD()) : MyFPSHUD;
	bool bHUDValid = MyFPSHUD && MyFPSHUD->CharacterOverlay && MyFPSHUD->CharacterOverlay->HealthBar && MyFPSHUD->CharacterOverlay->HealthText;
	if(bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		MyFPSHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		MyFPSHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void AMyFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	MyFPSHUD = Cast<AMyFPSHUD>(GetHUD());
	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogMyFPS, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void AMyFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}

bool AMyFPSPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
