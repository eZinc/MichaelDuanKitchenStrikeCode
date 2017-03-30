// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "TwoKitchenAIController.generated.h"

/**
 * 
 */
UENUM(BlueprintType)	
enum class EAIEnum : uint8
{
	AI_IDLE 	UMETA(DisplayName = "Idle"),
	AI_MOVINGTOLOCATION 	UMETA(DisplayName = "MovingToLocation"),
	AI_MOVINGTOOBJECT		UMETA(DisplayName = "MovingToUseObject"),
	AI_MOVINGTOLINENODE		UMETA(DisplayName = "MovingToLineNode")
};

UCLASS()
class KITCHENPROJECT_API ATwoKitchenAIController : public AAIController
{
	GENERATED_BODY()
	
	ATwoKitchenAIController();

public:
	UFUNCTION(BlueprintCallable, Category = AICharacter)
	virtual void MoveToOrder(FVector Destination);

	virtual void MoveToLineNode(class ALineNode* TargetNode);

	UFUNCTION(BlueprintCallable, Category = AICharacter)
	virtual void UseObjectOrder(AActor* Object);

	UFUNCTION(BlueprintCallable, Category = AICharacter)
	virtual bool CheckIfMoveIsLegal(FVector Destination);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
	EAIEnum StateEnum = EAIEnum::AI_IDLE;

	AActor* GoalObject;
	float GoalDistanceAllowed;
	FVector GoalLocation;
	class ALineNode* GoalLineNode;

	class ALineNode* CurrentLineNode;

	virtual void Tick(float deltaTime) override;

private:

};
