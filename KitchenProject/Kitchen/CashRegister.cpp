// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "KitchenManager.h"
#include "../NPC/TwoKitchenAIController.h"
#include "../KitchenProjectCharacter.h"
#include "LineNode.h"
#include "../Kitchen/KitchenManager.h"
#include "CashRegister.h"


// Sets default values
ACashRegister::ACashRegister()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetName(TEXT("Cash Register"));
	SetLabel(TEXT("Use Cash Register"));
}

// Called when the game starts or when spawned
void ACashRegister::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACashRegister::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ACashRegister::UseObject(AActor* Instigator)
{
	if (CustomerLineNode)
	{
		if (CustomerLineNode->Resident)
		{
			ATwoKitchenAIController* c = Cast<ATwoKitchenAIController>(CustomerLineNode->Resident->GetController());
			if (IsValid(c) && CustomerLineNode->IsAtCashierNode)
			{
				CustomerLineNode->IsAtCashierNode = false;
				GetKitchenManager()->KickCustomer(c, CustomerLineNode);

				// TODO: Multicast sound and have a UI effect
				UGameplayStatics::SpawnSoundAttached(CashierSuccessSound, GetRootComponent());

				GetKitchenManager()->AddMoney("sold food to customer", 11);
				GetKitchenManager()->AddKitchenRating(GetKitchenManager()->CustomerServedBonus);
			}
		}
	}
	
}

void ACashRegister::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACashRegister, KitchenManager);
}

float ACashRegister::CustomTakeDamage(bool bDidCrit, FVector LaunchVector, FVector HitLocation, float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	return 0;
}