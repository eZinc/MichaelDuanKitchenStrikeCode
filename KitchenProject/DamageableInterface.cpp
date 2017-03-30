// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "DamageableInterface.h"

UDamageableInterface::UDamageableInterface(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

float IDamageableInterface::CustomTakeDamage(bool bDidCrit, FVector LaunchVector, FVector HitLocation, float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	return 0;
}
