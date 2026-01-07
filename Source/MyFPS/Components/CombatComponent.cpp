// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "MyFPS/Weapon/Weapon.h"
#include "MyFPS/MyFPSCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
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

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr)return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetFirstPersonMesh()->GetSocketByName(FName("HandGrip_R"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetFirstPersonMesh());
	}
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->ShowPickupWidget(false);
}

