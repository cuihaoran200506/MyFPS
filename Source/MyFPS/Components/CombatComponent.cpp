// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "MyFPS/Weapon/Weapon.h"
#include "MyFPS/Character/MyFPSCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;


}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr)return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* FPHandSocket = Character->GetFirstPersonMesh()->GetSocketByName(FName("HandGrip_R"));
	if (FPHandSocket)  
	{
		FPHandSocket->AttachActor(EquippedWeapon, Character->GetFirstPersonMesh());
	}
	const USkeletalMeshSocket* TPHandSocket = Character->GetMesh()->GetSocketByName(FName("HandGrip_R"));
	if (TPHandSocket)
	{
		TPHandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
}


void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming; 
	ServerSetAiming(bIsAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
}
