// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "UnrealNetwork.h"
#include "../KitchenProjectCharacter.h"
#include "../Player/WeaponInventoryComponent.h"
#include "ThirdPersonWeapon.h"
#include "WorldWeapon.h"
#include "FirstPersonWeapon.h"


// Sets default values
AFirstPersonWeapon::AFirstPersonWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"), false);
	WeaponMesh->SetVisibility(false);
	RootComponent = WeaponMesh;
	SetReplicates(true);

	WeaponData = CreateDefaultSubobject<UWeaponDataComponent>(TEXT("WeaponData"), false);
	WeaponData->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AFirstPersonWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFirstPersonWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (bIsChargingDrop)
	{
		ImpulseFactor += ImpulseIncreasePerSecond * DeltaTime;
	}
}

// Needs to be called on the server and clients
void AFirstPersonWeapon::Initialize(AKitchenProjectCharacter* c, UWeaponDataComponent* w, AThirdPersonWeapon* ThirdWeapon)
{
	WeaponData->CopyWeaponDataComponent(w);
	// Attach shit to the person in first person
	SetOwner(c);
	AttachRootComponentToActor(c, NAME_None, EAttachLocation::SnapToTargetIncludingScale, false);
	WeaponMesh->AttachToComponent(c->GetArmsMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	OwnerCharacter = c;
	c->WeaponInventoryComponent->EquipWeapon(this);
	ThirdPersonWeapon = ThirdWeapon;
	ThirdPersonWeapon->Initialize(c, WeaponData);
	
}	

void AFirstPersonWeapon::FirePressed()
{
	FireEffects();
	ThirdPersonWeapon->Fire();
}

void AFirstPersonWeapon::FireReleased()
{

}

void AFirstPersonWeapon::AltFirePressed()
{

}
void AFirstPersonWeapon::AltFireReleased()
{

}

void AFirstPersonWeapon::SpecialPressed()
{

}
void AFirstPersonWeapon::SpecialReleased()
{

}

void AFirstPersonWeapon::ReloadPressed()
{
	ReloadEffects();
	ThirdPersonWeapon->Reload();
}
void AFirstPersonWeapon::ReloadReleased()
{

}

void AFirstPersonWeapon::SwitchIn()
{
	WeaponMesh->SetVisibility(true);
	SwitchInEffects();
	bCanFire = false;

	GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &AFirstPersonWeapon::FinishEquip, EquipTime);

	// TODO: Multicast
	OwnerCharacter->MeshWeaponIndex = GetWeaponData()->AnimationIndex;
	ThirdPersonWeapon->SwitchIn();
}

void AFirstPersonWeapon::FinishEquip()
{
	bCanFire = true;
}

void AFirstPersonWeapon::SwitchOut()
{
	WeaponMesh->SetVisibility(false);
	bCanFire = false;
	GetWorld()->GetTimerManager().ClearTimer(EquipTimer);

	ThirdPersonWeapon->SwitchOut();
}

void AFirstPersonWeapon::DropPressed()
{
	// Can't throw away hands
	// TODO: Have a message saying you can't
	if (WeaponData->SlotNumber == 0)
		return;

	ImpulseFactor = 50.0f;
	// TODO: Anticheat this so that the server is the one calculating the impuslse

	bIsChargingDrop = true;
}
void AFirstPersonWeapon::DropReleased()
{
	if (WeaponData->SlotNumber == 0)
		return;

	FVector ForwardVector = OwnerCharacter->GetFPCamera()->GetForwardVector();
	FVector FinalVector = ForwardVector * ImpulseFactor;
	CreateWorldWeapon(FinalVector);

	bIsChargingDrop = false;

	OwnerCharacter->WeaponInventoryComponent->SelectSlot(0);
}

bool AFirstPersonWeapon::CreateWorldWeapon_Validate(FVector ThrowVector)
{
	return true;
}

void AFirstPersonWeapon::CreateWorldWeapon_Implementation(FVector ThrowVector)
{
	FActorSpawnParameters param;
	AWorldWeapon* WorldWeapon = GetWorld()->SpawnActor<AWorldWeapon>(WeaponData->WorldWeaponClass, GetActorLocation(), GetActorRotation(), param);
	WorldWeapon->Initialize(WeaponData);
	WorldWeapon->WorldWeaponMesh->AddImpulse(ThrowVector);

	OwnerCharacter->WeaponInventoryComponent->WeaponArray[WeaponData->SlotNumber] = NULL;

	Destroy();
	ThirdPersonWeapon->Destroy();
}

void AFirstPersonWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFirstPersonWeapon, WeaponData);
	DOREPLIFETIME(AFirstPersonWeapon, OwnerCharacter);
	DOREPLIFETIME(AFirstPersonWeapon, ThirdPersonWeapon);
}