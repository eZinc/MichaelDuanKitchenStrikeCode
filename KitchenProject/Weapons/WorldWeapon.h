// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "../UsableInterface.h"
#include "Components/WeaponDataComponent.h"
#include "Components/WidgetComponent.h"
#include "../Kitchen/KitchenItemHealthWidget.h"
#include "WorldWeapon.generated.h"

UCLASS()
class KITCHENPROJECT_API AWorldWeapon : public AActor, public IUsableInterface, public IDamageableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldWeapon();

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* HealthBarWidget;

	UPROPERTY(EditDefaultsOnly, Category = HealthBar)
	int32 HealthBarType;

	UPROPERTY(EditDefaultsOnly, Category = HealthBar)
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = HealthBar)
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* HardDropSound;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* SoftDropSound;
	
	virtual void UseObject(AActor* Instigator);
	virtual void HoldUseObject(AActor* Instigator);


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	UBoxComponent* EquipCollider;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Initialize(class UWeaponDataComponent* data);
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WorldWeaponMesh;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	// Contains info about the weapon like mag size
	UPROPERTY(Replicated, EditDefaultsOnly)
	UWeaponDataComponent* WeaponData;

	virtual FString GetLabel() override;
	virtual FString GetName() override;

	virtual void HoveredOn() override;
	virtual void HoveredOff() override;

	UFUNCTION(BlueprintCallable, Category = Debug)
	virtual void GiveFirstPersonWeapon(class AKitchenProjectCharacter* c);

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
