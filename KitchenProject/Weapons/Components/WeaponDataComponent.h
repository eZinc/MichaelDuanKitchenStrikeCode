// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "WeaponDataComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KITCHENPROJECT_API UWeaponDataComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponDataComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	// Copy data from another one
	virtual void CopyWeaponDataComponent(UWeaponDataComponent* in);

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
	FString WeaponName;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
	int32 SlotNumber;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
	int32 AnimationIndex;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
	int32 CurrentMag;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
	int32 ReserveAmmo;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
	int32 MagSize;

	UPROPERTY(EditDefaultsOnly)
	class UClass* FirstPersonWeaponClass;

	UPROPERTY(EditDefaultsOnly)
	class UClass* ThirdPersonWeaponClass;

	UPROPERTY(EditDefaultsOnly)
	class UClass* WorldWeaponClass;
	
};
