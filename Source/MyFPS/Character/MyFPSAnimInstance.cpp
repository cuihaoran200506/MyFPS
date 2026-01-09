// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyFPSAnimInstance.h"
#include "MyFPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

	FRotator AimRotation = MyFPSCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MyFPSCharacter->GetVelocity());

	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 5.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = MyFPSCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = UKismetMathLibrary::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);
	float LeanWeight = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 100.f), FVector2D(0.f, 1.f), Speed);
	Lean = Lean * LeanWeight;
}
