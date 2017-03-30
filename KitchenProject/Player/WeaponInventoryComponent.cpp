// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "../Weapons/FirstPersonWeapon.h"
#include "../KitchenProjectCharacter.h"
#include "../Weapons/ThirdPersonWeapon.h"
#include "../Weapons/WorldWeapon.h"
#include "UnrealNetwork.h"
#include "WeaponInventoryComponent.h"


// Sets default values for this component's properties
UWeaponInventoryComponent::UWeaponInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	WeaponArray.SetNumZeroed(4);

	// ...
}


// Called when the game starts
void UWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWeaponInventoryComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void UWeaponInventoryComponent::SelectSlot(int32 slot)
{
	// hard coded limit return
	if (slot < 0 || slot > 3)
		return;
	
	// if there's no weapon, return
	if (WeaponArray[slot] == NULL)
	{
		return;
	}

	// MULTICAST THIS
	if (CurrentSelectedWeapon)
	{
		CurrentSelectedWeapon->SwitchOut();
	}

	CurrentSelectedWeapon = WeaponArray[slot];

	// play any switch in effects (show weapon and equip animation)
	if (CurrentSelectedWeapon)
	{
		CurrentSelectedWeapon->SwitchIn();
	}
		
}

void UWeaponInventoryComponent::Initialize(AKitchenProjectCharacter* c)
{
	OwningCharacter = c;

	if (GetWorld()->GetAuthGameMode())
	{
		FActorSpawnParameters param;
		param.Owner = c;
		AWorldWeapon* FistsWeapon = GetWorld()->SpawnActor<AWorldWeapon>(FistsWorldWeaponClass, FVector(0, 0, 0), FRotator(0, 0, 0), param);
		FistsWeapon->GiveFirstPersonWeapon(c);
	}
	
	SelectSlot(0);
}

void UWeaponInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponInventoryComponent, OwningCharacter);
	DOREPLIFETIME(UWeaponInventoryComponent, WeaponArray);
	DOREPLIFETIME(UWeaponInventoryComponent, CurrentSelectedWeapon);
}

bool UWeaponInventoryComponent::SlotIsOccupied(int32 slot)
{
	if (WeaponArray[slot] != NULL)
		return true;
	return false;
}

bool UWeaponInventoryComponent::EquipWeapon(AFirstPersonWeapon* weapon)
{
	// In case the weapon is bad
	if (weapon == NULL)
	{
		return false;
	}

	int32 slot = weapon->GetWeaponData()->SlotNumber;
	if (SlotIsOccupied(slot))
	{
		WeaponArray[slot]->DropReleased();
	}
	else
	{

	}
	WeaponArray[slot] = weapon;

	return true;
}
