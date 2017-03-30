// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "UnrealNetwork.h"
#include "../KitchenProjectCharacter.h"
#include "FirstPersonWeapon.h"
#include "ThirdPersonWeapon.h"
#include "../Player/WeaponInventoryComponent.h"
#include "Components/WeaponDataComponent.h"
#include "WorldWeapon.h"


// Sets default values
AWorldWeapon::AWorldWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WorldWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WorldWeaponMesh"), false);
	RootComponent = WorldWeaponMesh;
	SetReplicates(true);
	SetReplicateMovement(true);
	WorldWeaponMesh->SetSimulatePhysics(true);
	WorldWeaponMesh->SetNotifyRigidBodyCollision(true);
	WorldWeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WorldWeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	WeaponData = CreateDefaultSubobject<UWeaponDataComponent>(TEXT("WeaponData"), false);
	WeaponData->SetIsReplicated(true);

	EquipCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	EquipCollider->SetNotifyRigidBodyCollision(true);
	EquipCollider->AttachTo(WorldWeaponMesh);
	EquipCollider->OnComponentBeginOverlap.AddDynamic(this, &AWorldWeapon::OnOverlap);

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthBarWidget->AttachTo(WorldWeaponMesh);
}

// Called when the game starts or when spawned
void AWorldWeapon::BeginPlay()
{
	Super::BeginPlay();

	UKitchenItemHealthWidget* ItemWidget = Cast<UKitchenItemHealthWidget>(HealthBarWidget->GetUserWidgetObject());
	if (ItemWidget)
	{
		ItemWidget->InitializeHealthItem(WeaponData->WeaponName, MaxHealth, CurrentHealth, HealthBarType);
	}
}

FString AWorldWeapon::GetLabel()
{
	AController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (controller->GetPawn()->IsA(AKitchenProjectCharacter::StaticClass()))
	{
		AKitchenProjectCharacter* c = Cast<AKitchenProjectCharacter>(controller->GetPawn());
		if (c->WeaponInventoryComponent->SlotIsOccupied(WeaponData->SlotNumber))
		{
			return "Swap " + c->WeaponInventoryComponent->WeaponArray[WeaponData->SlotNumber]->GetWeaponData()->WeaponName + " for " + WeaponData->WeaponName;
		}
		else
		{
			return "Pick up " + WeaponData->WeaponName;
		}
	}

	return "Error.";
}

FString AWorldWeapon::GetName()
{
	return WeaponData->WeaponName;
}

// Called every frame
void AWorldWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AWorldWeapon::Initialize(UWeaponDataComponent* data)
{
	WeaponData->CopyWeaponDataComponent(data);
}


void AWorldWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWorldWeapon, WeaponData);
}

void AWorldWeapon::HoveredOn()
{
	UKitchenItemHealthWidget* ItemWidget = Cast<UKitchenItemHealthWidget>(HealthBarWidget->GetUserWidgetObject());
	if (ItemWidget)
	{
		ItemWidget->SetHealthItemVisibility(true);
	}

	Execute_BlueprintHoveredOn(this);
}

void AWorldWeapon::HoveredOff()
{
	UKitchenItemHealthWidget* ItemWidget = Cast<UKitchenItemHealthWidget>(HealthBarWidget->GetUserWidgetObject());
	if (ItemWidget)
	{
		ItemWidget->SetHealthItemVisibility(false);
	}

	Execute_BlueprintHoveredOff(this);
}

void AWorldWeapon::GiveFirstPersonWeapon(AKitchenProjectCharacter* c)
{
	FActorSpawnParameters param;
	param.Owner = c;
	AFirstPersonWeapon* FirstWeapon = GetWorld()->SpawnActor<AFirstPersonWeapon>(WeaponData->FirstPersonWeaponClass, FVector(0, 0, 0), FRotator(0, 0, 0), param);
	AThirdPersonWeapon* ThirdWeapon = GetWorld()->SpawnActor<AThirdPersonWeapon>(WeaponData->ThirdPersonWeaponClass, FVector(0, 0, 0), FRotator(0, 0, 0), param);
	FirstWeapon->Initialize(c, WeaponData, ThirdWeapon);
	Destroy();
}

void AWorldWeapon::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AWorldWeapon::UseObject(AActor* Instigator)
{
	if (Instigator->IsA(AKitchenProjectCharacter::StaticClass()))
	{
		AKitchenProjectCharacter* c = Cast<AKitchenProjectCharacter>(Instigator);
		GiveFirstPersonWeapon(c);
	}
}

void AWorldWeapon::HoldUseObject(AActor* Instigator)
{
	if (Instigator->IsA(AKitchenProjectCharacter::StaticClass()))
	{
		AKitchenProjectCharacter* c = Cast<AKitchenProjectCharacter>(Instigator);
		GiveFirstPersonWeapon(c);
	}
}

void AWorldWeapon::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetVelocity().Size() > 250.0f)
		UGameplayStatics::SpawnSoundAttached(HardDropSound, WorldWeaponMesh, "", FVector(0, 0, 0), EAttachLocation::KeepRelativeOffset, false, 1.0f, 1.0f, 0.0f);
	else if (GetVelocity().Size() > 180.0f)
		UGameplayStatics::SpawnSoundAttached(SoftDropSound, WorldWeaponMesh, "", FVector(0, 0, 0), EAttachLocation::KeepRelativeOffset, false, 1.0f, 1.0f, 0.0f);
}