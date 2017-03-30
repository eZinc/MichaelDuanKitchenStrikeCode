// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ThirdPersonWeapon.generated.h"

UCLASS()
class KITCHENPROJECT_API AThirdPersonWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThirdPersonWeapon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWeaponDataComponent* WeaponData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ThirdPersonWeaponMesh;

	virtual void SwitchIn();
	virtual void SwitchOut();

	virtual void Reload();
	virtual void Fire();

	UFUNCTION(BlueprintImplementableEvent, Category = ThirdPersonAnimation)
	void SwitchInEffects();

	UFUNCTION(BlueprintImplementableEvent, Category = ThirdPersonAnimation)
	void SwitchOutEffects();

	UFUNCTION(BlueprintImplementableEvent, Category = ThirdPersonAnimation)
	void ReloadEffects();

	UFUNCTION(BlueprintImplementableEvent, Category = ThirdPersonAnimation)
	void FireEffects();

	virtual void Initialize(class AKitchenProjectCharacter* c, class UWeaponDataComponent* data);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = ThirdPersonWeapon)
	class AKitchenProjectCharacter* OwnerCharacter;
	
	UPROPERTY(EditDefaultsOnly, Category = Socket)
	FName SocketName;
	
	
};
