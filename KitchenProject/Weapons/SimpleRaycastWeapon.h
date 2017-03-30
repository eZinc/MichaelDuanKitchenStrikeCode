// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/FirstPersonWeapon.h"
#include "SimpleRaycastWeapon.generated.h"

/**
 * 
 */
UCLASS()
class KITCHENPROJECT_API ASimpleRaycastWeapon : public AFirstPersonWeapon
{
	GENERATED_BODY()

	ASimpleRaycastWeapon();

	FTimerHandle ReloadTimer;
	FTimerHandle FinishedReloadTimer;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float ReloadTime;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float FullReloadTime;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	FName MuzzleSocketName;

	virtual void FirePressed() override;
	virtual void ReloadPressed() override;

	virtual void SwitchOut() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerNotifySwitchOut();

	virtual void ReloadLogic();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReloadLogic();

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	class USingleRaycastDamageComponent* RaycastComponent;
};
