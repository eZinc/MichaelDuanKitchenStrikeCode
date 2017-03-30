// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CustomerSpawnerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KITCHENPROJECT_API UCustomerSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCustomerSpawnerComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	class AKitchenManager* KitchenManager;

	void Initialize(class AKitchenManager* in);

	FTimerHandle SpawnTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	float DefaultMinCustomers;

	UPROPERTY(EditDefaultsOnly)
	float DefaultMaxCustomers;

	void StartSpawning();
	void StopSpawning();
	void SpawnCustomer();

	float GetNextTime();

	UPROPERTY(EditDefaultsOnly, Category = Class)
	TSubclassOf<class ATwoKitchenAICharacter> CustomerClass;

	UPROPERTY(EditDefaultsOnly, Category = Class)
	TSubclassOf<class ATwoKitchenAIController> ControllerClass;

	UPROPERTY(EditAnywhere)
	ATargetPoint* StartLocationDebug;
	
};
