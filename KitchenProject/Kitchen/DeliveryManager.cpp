// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "UnrealNetwork.h"
#include "KitchenManager.h"
#include "DeliveryManager.h"


// Sets default values
ADeliveryManager::ADeliveryManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADeliveryManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeliveryManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ADeliveryManager::ServerOrderItem_Implementation(FName Name, UClass* ItemClass, int32 quantity, int32 price, int32 MinutesToDeliver)
{
	int32 totalPrice = quantity * price;

	FString TotalString = "bought " + Name.ToString() + " (" + FString::FromInt(quantity) + ")";
	KitchenManager->AddMoney(TotalString, -totalPrice);

	for (int i = 0; i < quantity; i++)
	{
		FActorSpawnParameters params;
		AActor* Item = GetWorld()->SpawnActor<AActor>(ItemClass, DeliverySpot->GetActorLocation(), DeliverySpot->GetActorRotation(), params);
	}

}

bool ADeliveryManager::ServerOrderItem_Validate(FName Name, UClass* ItemClass, int32 quantity, int32 price, int32 MinutesToDeliver)
{
	return true;
}