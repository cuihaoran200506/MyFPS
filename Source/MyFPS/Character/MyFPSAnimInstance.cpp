// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyFPSAnimInstance.h"
#include "MyFPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMyFPSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MyFPSCharacter = Cast<AMyFPSCharacter>(TryGetPawnOwner());
}

void UMyFPSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (MyFPSCharacter==nullptr)
	{
		MyFPSCharacter = Cast<AMyFPSCharacter>(TryGetPawnOwner());
	}
	if (MyFPSCharacter == nullptr) return;

	FVector Velocity = MyFPSCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = MyFPSCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = MyFPSCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bWeaponEquipped = MyFPSCharacter->IsWeaponEquipped();
	bIsCrouched = MyFPSCharacter->bIsCrouched;
	bAiming = MyFPSCharacter->IsAiming();
}
