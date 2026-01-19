// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Projectile.generated.h"

UCLASS()
class MYFPS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(
		UPrimitiveComponent* HitComp,
		AActor*OtherActor, 
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayImpactEffects(FVector_NetQuantize Location, FVector_NetQuantize Normal);

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

private:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TracerNiagara;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ImpactNiagara;

	UPROPERTY(EditAnywhere)
	class USoundBase* ImpactSound;
public:	
	
};
