// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DeliveryManager.generated.h"

UCLASS()
class KITCHENPROJECT_API ADeliveryManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeliveryManager();

	UPROPERTY(EditAnywhere)
	class AKitchenManager* KitchenManager;

	UPROPERTY(EditAnywhere)
	ATargetPoint* DeliverySpot;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(Server, Reliable, WithValidation, Category = Computer)
		void ServerOrderItem(FName Name, UClass* ItemClass, int32 quantity, int32 price, int32 MinutesToDeliver);
	
};
