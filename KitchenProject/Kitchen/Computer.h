// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "../UsableInterface.h"
#include "../Weapons/WorldWeapon.h"
#include "Computer.generated.h"

UCLASS()
class KITCHENPROJECT_API AComputer : public AWorldWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AComputer();

	UPROPERTY(EditDefaultsOnly)
	FString Password;

	void ChangePassword(const FString& NewPassword)
	{
		Password = NewPassword;
	}

	UPROPERTY(EditAnywhere)
	class AKitchenManager* KitchenManager;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void UseObject(AActor* Instigator) override;
	virtual void HoldUseObject(AActor* Instigator);

	virtual FString GetLabel() override;

	UPROPERTY(Replicated)
	bool bIsInUse = false;

	UFUNCTION(Server, Reliable, WithValidation)
	void RequestUseComputer(class ATwoKitchenPlayerController* cont);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastShowUsage(bool ShouldLight);

	UFUNCTION(BlueprintImplementableEvent)
	void LightUpScreen();

	UFUNCTION(BlueprintImplementableEvent)
	void DarkenScreen();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
};
