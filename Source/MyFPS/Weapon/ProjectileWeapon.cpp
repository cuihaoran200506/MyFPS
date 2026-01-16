// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	if (!HasAuthority())return;
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector SocketLocation = SocketTransform.GetLocation();

		FVector TargetLocation = HitTarget;
		if (TargetLocation.IsNearlyZero())
		{
			FVector ForwardDir = InstigatorPawn->GetControlRotation().Vector();
			TargetLocation = SocketLocation + (ForwardDir * 10000.f);
		}

		FVector ToTarget = TargetLocation - SocketLocation;
		FRotator TargetRotation = ToTarget.Rotation();

		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			UWorld * World = GetWorld(); 
			if (World) 
			{
				World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
			}
		}
	}
}
   