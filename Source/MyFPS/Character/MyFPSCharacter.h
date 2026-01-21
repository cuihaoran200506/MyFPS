 // Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MyFPS/Interfaces/InteractWithCrosshairsInterface.h"
#include "MyFPSCharacter.generated.h"


class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AMyFPSCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	virtual void BeginPlay() override;
	//Camera settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Camera")
	float StandHeight = 64.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Camera")
	float CrouchHeight = 35.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Camera")
	float CameraInterpSpeed = 15.f;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* AdsAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* FireAction;

public:
	AMyFPSCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents()override;
	void PlayFireMontage(bool bAiming);
	void PlayElimMontage(bool bEquipped);
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpEnd();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoEquip();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoCrouch();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoAds();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoFireStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoFireEnd();

	void AimOffset(float DeltaTime);

	void PlayHitReactMontage();
	UFUNCTION()
	void ReceiveDamage(
		AActor* DamagedActor,
		float Damage,
		UDamageType const* DamageType,
		class AController* InstigatorController,
		AActor* DamageCauser
		);
	void UpdateHUDHealth();
protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;
	UFUNCTION(Server, Reliable)
	void ServerDoEquip();

	/*
	FRotator StartingAimRotation;
	float AO_Yaw;
	*/
	float AO_Pitch;
	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;
	UFUNCTION()
	void OnRep_Health();

	class AMyFPSPlayerController* MyFPSPlayerController;

	bool bElimmed=false;
	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 2.0f;
	void ElimTimerFinished();

public:

	void SetOverlappingWeapon(AWeapon* Weapon);

	bool IsWeaponEquipped();

	bool IsAiming();
	/*
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	*/
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	AWeapon* GetEquippedWeapon();

	FVector GetHitTarget() const;

	FORCEINLINE UCameraComponent* GetCamera() const { return FirstPersonCameraComponent; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
};