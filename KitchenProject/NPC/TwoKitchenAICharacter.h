// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "KitchenProjectCharacter.h"
#include "../UsableInterface.h"
#include "UnrealNetwork.h"
#include "TwoKitchenAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class KITCHENPROJECT_API ATwoKitchenAICharacter : public AKitchenProjectCharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated)
	bool isSelected = false;

	UPROPERTY(Replicated)
	AKitchenProjectCharacter* KitchenOwner;

	void Die(AActor* Killer, FVector LaunchVector, FVector HitLocation) override;

	class AKitchenManager* CurrentKitchen;

	UPROPERTY(EditAnywhere, Replicated)
	int32 PlayerIDCanOwn = -1;

	void CantReachDestination();
	void Select(AKitchenProjectCharacter* Instigator);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSelectMinion(class AKitchenProjectCharacter* own);

	void MovingToDestination()
	{
		NetworkedPlaySound(AffirmativeGoing, true);
	}

	UFUNCTION(BlueprintCallable, Category = LineNode)
	void GoToLineNode(class ALineNode* TargetNode);

	void SelectFail(AKitchenProjectCharacter* Instigator);
	void MoveToOrder(FVector Destination, AKitchenProjectCharacter* Instigator);
	void ReachedDestination();
	bool CanBossAround(AKitchenProjectCharacter* Instigator);

	UPROPERTY(EditDefaultsOnly, Category = Voices)
	USoundBase* ReachedLocation;

	UPROPERTY(EditDefaultsOnly, Category = Voices)
	USoundBase* CantReachLocation;
	
	UPROPERTY(EditDefaultsOnly, Category = Voices)
	USoundBase* AffirmativeWaiting;

	UPROPERTY(EditDefaultsOnly, Category = Voices)
	USoundBase* AffirmativeGoing;
	
	UPROPERTY(EditDefaultsOnly, Category = Voices)
	USoundBase* NegativeNotBoss;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString DisplayName;

	UFUNCTION(BlueprintImplementableEvent)
	void DrawDamageUI(bool bDidCrit, AController* c, float UpdatedHealth, float Damage, FVector HitLocation);

	virtual FString GetLabel() override;
	virtual FString GetName() override;
	virtual void UseObject(AActor* Instigator) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
};
