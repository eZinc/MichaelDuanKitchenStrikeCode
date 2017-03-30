// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SimpleCar.generated.h"

UCLASS()
class KITCHENPROJECT_API ASimpleCar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimpleCar();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float Speed;

	FTimerHandle DeathTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float Lifetime;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float Distance;

	void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditDefaultsOnly, Category = Hit)
	float ImpulseStrength;

	UPROPERTY(EditDefaultsOnly, Category = Hit)
	class USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, Category = Hit)
	float HitDamage;

	void Despawn();

private:

	
	
};
