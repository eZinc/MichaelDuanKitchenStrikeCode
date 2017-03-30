// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "KitchenManager.h"
#include "UnrealNetwork.h"
#include "../NPC/TwoKitchenAICharacter.h"
#include "../NPC/TwoKitchenAIController.h"
#include "CustomerSpawnerComponent.h"


// Sets default values for this component's properties
UCustomerSpawnerComponent::UCustomerSpawnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCustomerSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCustomerSpawnerComponent::Initialize(AKitchenManager* in)
{
	KitchenManager = in;
}

// Called every frame
void UCustomerSpawnerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void UCustomerSpawnerComponent::StartSpawning()
{
	float time = GetNextTime();
	KitchenManager->GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &UCustomerSpawnerComponent::SpawnCustomer, time, false);
}

void UCustomerSpawnerComponent::StopSpawning()
{
	KitchenManager->GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

float UCustomerSpawnerComponent::GetNextTime()
{
	return FMath::FRandRange(DefaultMinCustomers * 1/KitchenManager->KitchenRating, DefaultMaxCustomers * 1 / KitchenManager->KitchenRating);
}

void UCustomerSpawnerComponent::SpawnCustomer()
{
	if (!KitchenManager->IsKitchenFull())
	{
		ATwoKitchenAICharacter* c = GetWorld()->SpawnActor<ATwoKitchenAICharacter>(CustomerClass, StartLocationDebug->GetActorLocation(), StartLocationDebug->GetActorRotation());
		c->CurrentKitchen = KitchenManager;

		ATwoKitchenAIController* cont = GetWorld()->SpawnActor<ATwoKitchenAIController>(ControllerClass, StartLocationDebug->GetActorLocation(), StartLocationDebug->GetActorRotation());
		cont->Possess(c);

		KitchenManager->AcceptCustomer(cont);
	}

	float time = GetNextTime();
	KitchenManager->GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &UCustomerSpawnerComponent::SpawnCustomer, time, false);
}