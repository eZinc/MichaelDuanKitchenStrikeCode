// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "../KitchenProjectCharacter.h"
#include "Components/WeaponDataComponent.h"
#include "UnrealNetwork.h"
#include "ThirdPersonWeapon.h"


// Sets default values
AThirdPersonWeapon::AThirdPersonWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ThirdPersonWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonWeaponMesh"), false);
	ThirdPersonWeaponMesh->SetOwnerNoSee(true);
	RootComponent = ThirdPersonWeaponMesh;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AThirdPersonWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThirdPersonWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AThirdPersonWeapon::SwitchIn()
{
	SwitchInEffects();
}

// TODO: Multicast this
void AThirdPersonWeapon::Fire()
{
	FireEffects();
}

// TODO: Multicast this
void AThirdPersonWeapon::Reload()
{
	ReloadEffects();
}

// TODO: Multicast this
void AThirdPersonWeapon::SwitchOut()
{
	SwitchOutEffects();
}

void AThirdPersonWeapon::Initialize(AKitchenProjectCharacter* c, UWeaponDataComponent* data)
{
	WeaponData = data;
	SetOwner(c);
	AttachRootComponentToActor(c, NAME_None, EAttachLocation::SnapToTargetIncludingScale, false);
	ThirdPersonWeaponMesh->AttachToComponent(c->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	OwnerCharacter = c;
}

void AThirdPersonWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AThirdPersonWeapon, OwnerCharacter);
}