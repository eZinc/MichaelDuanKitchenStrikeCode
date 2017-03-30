// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "SingleRaycastDamageComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KITCHENPROJECT_API USingleRaycastDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USingleRaycastDamageComponent();

	UPROPERTY(EditDefaultsOnly)
	float FalloffDistance;

	UPROPERTY(EditDefaultsOnly)
	float DamageLostPerUnit;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* BrickImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float ImpulseAmount;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float Range;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	class UParticleSystem* ImpactFX;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* TracerParticle;

	UPROPERTY(Transient)
	UParticleSystemComponent* TrailComponent;

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
	virtual bool ShootRaycast(class UWeaponDataComponent* data, FVector MuzzleLocation, FVector ForwardVector, FVector StartingPosition, class AKitchenProjectCharacter* c);

	virtual void RaycastEffect(FVector MuzzleLocation, FHitResult Hit);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestRaycast(UWeaponDataComponent* data, FVector MuzzleLocation, FHitResult Hit, FVector ForwardVector);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRaycastEffect(FVector MuzzleLocation, FHitResult Hit);
};
