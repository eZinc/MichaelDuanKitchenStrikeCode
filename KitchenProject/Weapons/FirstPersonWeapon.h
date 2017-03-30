// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/WeaponDataComponent.h"
#include "ThirdPersonWeapon.h"
#include "FirstPersonWeapon.generated.h"

UCLASS()
class KITCHENPROJECT_API AFirstPersonWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFirstPersonWeapon();

	FTimerHandle EquipTimer;

	UPROPERTY(EditAnywhere, Category = Stats)
	float EquipTime;

	bool bCanFire = false;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Weapon model
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

	// Holds information such as damage type, mag size, ammo count
	UPROPERTY(Replicated, BlueprintReadOnly)
	UWeaponDataComponent* WeaponData;

	UPROPERTY(Replicated, BlueprintReadOnly)
	class AKitchenProjectCharacter* OwnerCharacter;

	UWeaponDataComponent* GetWeaponData()
	{
		return WeaponData;
	}

	// Weapon that is added to the 3rd person model
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AThirdPersonWeapon* ThirdPersonWeapon;

	AThirdPersonWeapon* GetThirdPersonWeapon()
	{
		return ThirdPersonWeapon;
	}
	
	void SetCanFire()
	{
		bCanFire = true;
	}

	void SetCannotFire()
	{
		bCanFire = false;
	}

	virtual void FinishEquip();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FName SocketName;

	// Attach the weapon to the character
	UFUNCTION(BlueprintCallable, Category = WeaponDebug)
	virtual void Initialize(class AKitchenProjectCharacter* c, class UWeaponDataComponent* w, class AThirdPersonWeapon* ThirdWeapon);

	virtual void FirePressed();
	virtual void FireReleased();

	UFUNCTION(BlueprintImplementableEvent, Category = WeaponEffects)
	void FireEffects();

	UFUNCTION(BlueprintImplementableEvent, Category = WeaponEffects)
	void ReloadEffects();
	
	UFUNCTION(BlueprintImplementableEvent, Category = WeaponEffects)
	void SwitchInEffects();

	virtual void AltFirePressed();
	virtual void AltFireReleased();

	virtual void SpecialPressed();
	virtual void SpecialReleased();

	virtual void ReloadPressed();
	virtual void ReloadReleased();

	virtual void DropPressed();
	virtual void DropReleased();

	virtual void SwitchIn();
	virtual void SwitchOut();

	UPROPERTY(EditDefaultsOnly)
	class UClass* WorldWeaponClass;
	
	float ImpulseFactor;

	UPROPERTY(EditDefaultsOnly)
	float ImpulseIncreasePerSecond;

	// Creates a world weapon using the current weapondata
	UFUNCTION(Server, Reliable, WithValidation)
	void CreateWorldWeapon(FVector ThrowVector);

	bool bIsChargingDrop = false;
	
};
