// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "WeaponInventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KITCHENPROJECT_API UWeaponInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponInventoryComponent();

	UPROPERTY(EditDefaultsOnly)
	class UClass* FistsWorldWeaponClass;

	UPROPERTY(Replicated)
	TArray<class AFirstPersonWeapon*>WeaponArray;

	UPROPERTY(Replicated, BlueprintReadOnly)
	class AFirstPersonWeapon* CurrentSelectedWeapon;

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	virtual void SelectSlot(int32 slot);
	UPROPERTY(Replicated)
	class AKitchenProjectCharacter* OwningCharacter;

	virtual void Initialize(class AKitchenProjectCharacter* c);

	virtual bool EquipWeapon(class AFirstPersonWeapon* weapon);

	virtual bool SlotIsOccupied(int32 slot);
	
};
