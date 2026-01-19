// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyFPSCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyFPS/MyFPS.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "MyFPS/Weapon/Weapon.h"
#include "MyFPS/Components/CombatComponent.h"

AMyFPSCharacter::AMyFPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(20.0f, 0.0f, StandHeight));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.9f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true); 

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	FirstPersonMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);
}

void AMyFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    //adapt to firing
	if (FirstPersonCameraComponent)
	{
		float TargetZ = bIsCrouched ? CrouchHeight : StandHeight;
		FVector CurrentRelativeLocation = FirstPersonCameraComponent->GetRelativeLocation();
		if (IsWeaponEquipped() && !IsAiming())
		{
			TargetZ -= 10.0f; 
		}
		if (GetVelocity().Size() > 10.f && IsAiming())
		{
			if (bIsCrouched)
			{
				TargetZ += 20.0f;
			}
			else
			{
				TargetZ -= 15.0f;
			}
		}
		float NewZ = FMath::FInterpTo(CurrentRelativeLocation.Z, TargetZ, DeltaTime, 8.f);
		FirstPersonCameraComponent->SetRelativeLocation(FVector(CurrentRelativeLocation.X, CurrentRelativeLocation.Y, NewZ));
	}
	AimOffset(DeltaTime);
}

void AMyFPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;

	}
}

void AMyFPSCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)return;

	UAnimInstance* AnimInstanceFP = FirstPersonMesh->GetAnimInstance();
	UAnimInstance* AnimInstanceTP = GetMesh()->GetAnimInstance();
	if (AnimInstanceFP&&FireWeaponMontage)
	{
		AnimInstanceFP->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstanceFP->Montage_JumpToSection(SectionName);
	}
	if (AnimInstanceTP && FireWeaponMontage) 
	{
		AnimInstanceTP->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstanceTP->Montage_JumpToSection(SectionName);
	}
}

void AMyFPSCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)return;

	UAnimInstance* AnimInstanceFP = FirstPersonMesh->GetAnimInstance();
	UAnimInstance* AnimInstanceTP = GetMesh()->GetAnimInstance();
	if (AnimInstanceFP && HitReactMontage)
	{
		AnimInstanceFP->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstanceFP->Montage_JumpToSection(SectionName);
	}
	if (AnimInstanceTP && HitReactMontage)
	{
		AnimInstanceTP->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstanceFP->Montage_JumpToSection(SectionName);
	}
}

void AMyFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyFPSCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyFPSCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyFPSCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyFPSCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMyFPSCharacter::LookInput);

		//Equippping
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &AMyFPSCharacter::DoEquip);
		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMyFPSCharacter::DoCrouch);
		//ADS
		EnhancedInputComponent->BindAction(AdsAction, ETriggerEvent::Started, this, &AMyFPSCharacter::DoAds);
		//Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMyFPSCharacter::DoFireStart);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AMyFPSCharacter::DoFireEnd);
	}
	else
	{
		UE_LOG(LogMyFPS, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMyFPSCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void AMyFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION (AMyFPSCharacter, OverlappingWeapon,COND_OwnerOnly); 
	DOREPLIFETIME(AMyFPSCharacter, Health);
}

void AMyFPSCharacter::MulticastHit_Implementation()
{
	PlayHitReactMontage();
}

void AMyFPSCharacter::OnRep_Health()
{
}

void AMyFPSCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool AMyFPSCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool AMyFPSCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}
 
AWeapon* AMyFPSCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr)return nullptr;
	else return Combat->EquippedWeapon;
}

FVector AMyFPSCharacter::GetHitTarget() const
{
	if(Combat==nullptr)return FVector();
	return Combat->HitTarget;
}



void AMyFPSCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AMyFPSCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AMyFPSCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMyFPSCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AMyFPSCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}
    
void AMyFPSCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void AMyFPSCharacter::DoEquip()
{
	if (Combat)
	{
		if(HasAuthority())
			Combat->EquipWeapon(OverlappingWeapon);
		else
			ServerDoEquip(); 
	}
	
}

void AMyFPSCharacter::ServerDoEquip_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void AMyFPSCharacter::DoCrouch()
{
	if (bIsCrouched)
		UnCrouch();
	else
		Crouch();
}

void AMyFPSCharacter::DoAds()
{
	if (Combat)
	{
		Combat->SetAiming(!Combat->bAiming);
	}
}



void AMyFPSCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr)return;
	/*
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool IsInAir = GetCharacterMovement()->IsFalling();
	if (Speed == 0.f && !IsInAir)
	{
		FRotator CurrentAimRotation= FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
	} 
	if (Speed > 0.f || IsInAir)
	{
		StartingAimRotation=FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
	}
	*/
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}


void AMyFPSCharacter::DoFireStart()
{
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void AMyFPSCharacter::DoFireEnd()
{
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}
