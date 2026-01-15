// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; 
	SetReplicateMovement(true);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (TracerNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			TracerNiagara,
			CollisionBox,        
			FName(),               
			FVector::ZeroVector,    
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget, 
			true                       
		);
	}
	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	float DistanceTraveled = FVector::Dist(GetActorLocation(), FVector::ZeroVector);

	UE_LOG(LogTemp, Warning, TEXT("Hit Detected! Actor: %s | Distance from Origin: %f | HitPoint: %s"),
		OtherActor ? *OtherActor->GetName() : TEXT("None"),
		DistanceTraveled,
		*Hit.ImpactPoint.ToString());
	if (HasAuthority())
	{
		Multicast_PlayImpactEffects(Hit.ImpactPoint, Hit.ImpactNormal);
		SetActorHiddenInGame(true);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetLifeSpan(2.0f);
	}
}

void AProjectile::Multicast_PlayImpactEffects_Implementation(FVector_NetQuantize Location, FVector_NetQuantize Normal)
{
	
	if (ImpactNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactNiagara, Location, Normal.Rotation());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Location);
	}
}
// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



