// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SimpleCarSpawner.generated.h"

UCLASS()
class KITCHENPROJECT_API ASimpleCarSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimpleCarSpawner();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnCar();

	void StartSpawningCars();
	void StopSpawningCars();

	UPROPERTY(EditAnywhere, Category = Class)
	TSubclassOf<class ASimpleCar> CarClass;

	UPROPERTY(EditAnywhere, Category = Stats)
	float DayMinDelay;

	UPROPERTY(EditAnywhere, Category = Stats)
	float DayMaxDelay;

	UPROPERTY(EditAnywhere, Category = Stats)
	float NightMinDelay;

	UPROPERTY(EditAnywhere, Category = Stats)
	float NightMaxDelay;

private:
	FTimerHandle SpawnTimerHandle;

	
};
