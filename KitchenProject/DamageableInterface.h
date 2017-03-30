// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DamageableInterface.generated.h"
/**
 * 
 */
UINTERFACE()
class UDamageableInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class KITCHENPROJECT_API IDamageableInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	virtual float CustomTakeDamage(bool bDidCrit, FVector LaunchVector, FVector HitLocation, float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);
};
