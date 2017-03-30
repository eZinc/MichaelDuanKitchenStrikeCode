// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "SimpleCar.h"
#include "../TwoKitchenGamestate.h"
#include "SimpleCarSpawner.h"


// Sets default values
ASimpleCarSpawner::ASimpleCarSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASimpleCarSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ASimpleCarSpawner::StartSpawningCars() 
{
	float random = FMath::FRandRange(NightMinDelay, NightMaxDelay);
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASimpleCarSpawner::SpawnCar, random, false);
}

void ASimpleCarSpawner::StopSpawningCars()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}


void ASimpleCarSpawner::SpawnCar()
{
	ATwoKitchenGamestate* g = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());
	if (g)
	{
		float random;
		// Fewer cars spawn during the "night"
		// TODO: Use a curve instead of this hard coded shit
		int32 HourOfDay = g->GetHour();
		if (HourOfDay < 8 || HourOfDay > 20)
		{
			random = FMath::FRandRange(NightMinDelay, NightMaxDelay);
		}
		else
		{
			random = FMath::FRandRange(DayMinDelay, DayMaxDelay);
		}

		GetWorld()->SpawnActor<ASimpleCar>(CarClass, GetActorLocation(), GetActorRotation());
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASimpleCarSpawner::SpawnCar, random, false);
	}
}
