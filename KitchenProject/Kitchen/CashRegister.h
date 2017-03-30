// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "../UsableInterface.h"
#include "UnrealNetwork.h"
#include "../DamageableInterface.h"
#include "../Weapons/WorldWeapon.h"
#include "CashRegister.generated.h"

UCLASS()
class KITCHENPROJECT_API ACashRegister : public AWorldWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACashRegister();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void UseObject(AActor* Instigator);

	UPROPERTY(EditDefaultsOnly)
	USoundBase* CashierSuccessSound;

	UPROPERTY(EditAnywhere)
	class ALineNode* CustomerLineNode;

	UPROPERTY(EditAnywhere, Replicated)
	class AKitchenManager* KitchenManager;

	class AKitchenManager* GetKitchenManager()
	{
		return KitchenManager;
	}

	virtual float CustomTakeDamage(bool bDidCrit, FVector LaunchVector, FVector HitLocation, float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	
};
