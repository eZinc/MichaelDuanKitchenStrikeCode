// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "UnrealNetwork.h"
#include "WeaponDataComponent.h"


// Sets default values for this component's properties
UWeaponDataComponent::UWeaponDataComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UWeaponDataComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWeaponDataComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void UWeaponDataComponent::CopyWeaponDataComponent(UWeaponDataComponent* in)
{
	this->CurrentMag = in->CurrentMag;
	this->ReserveAmmo = in->ReserveAmmo;
	this->MagSize = in->MagSize;
	this->SlotNumber = in->SlotNumber;
	this->AnimationIndex = in->AnimationIndex;
	this->WeaponName = in->WeaponName;
	this->FirstPersonWeaponClass = in->FirstPersonWeaponClass;
	this->ThirdPersonWeaponClass = in->ThirdPersonWeaponClass;
	this->WorldWeaponClass = in->WorldWeaponClass;
}


void UWeaponDataComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponDataComponent, CurrentMag);
	DOREPLIFETIME(UWeaponDataComponent, ReserveAmmo);
	DOREPLIFETIME(UWeaponDataComponent, MagSize);
	DOREPLIFETIME(UWeaponDataComponent, SlotNumber);
	DOREPLIFETIME(UWeaponDataComponent, AnimationIndex);
	DOREPLIFETIME(UWeaponDataComponent, WeaponName);
}

