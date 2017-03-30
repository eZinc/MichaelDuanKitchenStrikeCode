// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "Components/SingleRaycastDamageComponent.h"
#include "../KitchenProjectCharacter.h"
#include "SimpleRaycastWeapon.h"

ASimpleRaycastWeapon::ASimpleRaycastWeapon():Super()
{
	RaycastComponent = CreateDefaultSubobject<USingleRaycastDamageComponent>(TEXT("RaycastComponent"), false);
	RaycastComponent->SetIsReplicated(true);
}

void ASimpleRaycastWeapon::FirePressed()
{
	if (bCanFire == false)
		return;

	FVector ForwardVector = OwnerCharacter->GetFPCamera()->GetForwardVector();
	FVector StartingVector = OwnerCharacter->GetFPCamera()->GetComponentLocation();

	AKitchenProjectCharacter* c = Cast<AKitchenProjectCharacter>(GetOwner());

	if (c)
	{
		if (RaycastComponent->ShootRaycast(WeaponData, WeaponMesh->GetSocketLocation(MuzzleSocketName), ForwardVector, StartingVector, c))
		{
			Super::FirePressed();
		}
		else
		{
			ReloadPressed();
		}
	}
}

void ASimpleRaycastWeapon::ReloadPressed()
{
	// Can't reload if no reserve ammo
	if (WeaponData->ReserveAmmo <= 0 || bCanFire == false || WeaponData->MagSize == WeaponData->CurrentMag)
	{
		// TODO: Play a sound?
		return;
	}

	Super::ReloadPressed();

	// TODO: Disallow shooting while reloading

	bCanFire = false;

	// Clientside for GUI
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &ASimpleRaycastWeapon::ReloadLogic, ReloadTime);
	GetWorld()->GetTimerManager().SetTimer(FinishedReloadTimer, this, &ASimpleRaycastWeapon::SetCanFire, FullReloadTime);

	// Serverside actual calculation
	ServerReloadLogic();
}

void ASimpleRaycastWeapon::ServerReloadLogic_Implementation()
{
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &ASimpleRaycastWeapon::ReloadLogic, ReloadTime);
}

bool ASimpleRaycastWeapon::ServerReloadLogic_Validate()
{
	if (WeaponData->ReserveAmmo <= 0)
	{
		return false;
	}
	return true;
}

void ASimpleRaycastWeapon::ReloadLogic()
{
	WeaponData->ReserveAmmo += WeaponData->CurrentMag;
	WeaponData->ReserveAmmo -= WeaponData->MagSize;
	WeaponData->CurrentMag = WeaponData->MagSize;
	if (WeaponData->ReserveAmmo <= 0)
	{
		WeaponData->CurrentMag += WeaponData->ReserveAmmo;
		WeaponData->ReserveAmmo = 0;
	}
}

void ASimpleRaycastWeapon::SwitchOut()
{
	Super::SwitchOut();

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	GetWorld()->GetTimerManager().ClearTimer(FinishedReloadTimer);

	// TODO: Disable the shooting lock while reloading

	// TODO: Delete the reload timer when switching out
}

void ASimpleRaycastWeapon::ServerNotifySwitchOut_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
}

bool ASimpleRaycastWeapon::ServerNotifySwitchOut_Validate()
{
	return true;
}