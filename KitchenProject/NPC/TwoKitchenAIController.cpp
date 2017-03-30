// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "../NPC/TwoKitchenAICharacter.h"
#include "../KitchenProjectCharacter.h"
#include "../Kitchen/LineNode.h"
#include "TwoKitchenAIController.h"

ATwoKitchenAIController::ATwoKitchenAIController()
{
	
}

void ATwoKitchenAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (HasAuthority())
	{
		if (StateEnum == EAIEnum::AI_IDLE)
		{
			return;
		}
		else if (StateEnum == EAIEnum::AI_MOVINGTOOBJECT)
		{
			FVector MyLocation = GetPawn()->GetActorLocation();
			float distance = (GoalLocation - MyLocation).Size();
			if (distance <= GoalDistanceAllowed)
			{
				IUsableInterface* u = Cast<IUsableInterface>(GoalObject);
				if (u)
				{
					u->UseObject(GetPawn());
				}
				StateEnum = EAIEnum::AI_IDLE;
			}
		}
		else if (StateEnum == EAIEnum::AI_MOVINGTOLOCATION)
		{
			return;
		}
		else if (StateEnum == EAIEnum::AI_MOVINGTOLINENODE)
		{
			FVector distanceVector = GoalLineNode->GetActorLocation() - GetPawn()->GetActorLocation();
			float distance = distanceVector.Size();
			if (distance <= 95.0f)
			{
				ATwoKitchenAICharacter* character = Cast<ATwoKitchenAICharacter>(GetPawn());
				if (GoalLineNode->IsCashierNode)
				{
					// do stuff that says ok
					GoalLineNode->IsAtCashierNode = true;
					StateEnum = EAIEnum::AI_IDLE;
				}
				else if(GoalLineNode->IsDeleteNode)
				{
					character->Destroy();
				}
				else if (GoalLineNode->NextNode)
				{
					MoveToLineNode(GoalLineNode->NextNode);
				}

			}
		}
	}
}

void ATwoKitchenAIController::MoveToOrder(FVector Destination)
{
	ATwoKitchenAICharacter* character = Cast<ATwoKitchenAICharacter>(GetPawn());

	if (CheckIfMoveIsLegal(Destination))
	{
		StateEnum = EAIEnum::AI_MOVINGTOLOCATION;
		character->MovingToDestination();
		GoalLocation = Destination;
		MoveToLocation(GoalLocation, 1.0f, false, true, false);
	}
	else
	{
		StateEnum = EAIEnum::AI_IDLE;
		character->CantReachDestination();
	}
}

void ATwoKitchenAIController::UseObjectOrder(AActor* Object)
{
	ATwoKitchenAICharacter* character = Cast<ATwoKitchenAICharacter>(GetPawn());
	IUsableInterface* u = Cast<IUsableInterface>(Object);

	if (CheckIfMoveIsLegal(Object->GetActorLocation()))
	{
		StateEnum = EAIEnum::AI_MOVINGTOOBJECT;
		character->MovingToDestination();

		GoalObject = Object;
		GoalLocation = u->GetAIUseLocation(GetPawn());
		GoalDistanceAllowed = u->GetAIUseDistance(GetPawn());
		MoveToLocation(GoalLocation, 1.0f, false, true, false);
	}
	else
	{
		StateEnum = EAIEnum::AI_IDLE;
		character->CantReachDestination();
	}
}

bool ATwoKitchenAIController::CheckIfMoveIsLegal(FVector Destination)
{
	FPathFindingQuery query;
	query.StartLocation = GetPawn()->GetActorLocation();
	query.EndLocation = Destination;
	query.Owner = this;
	query.NavData = GetWorld()->GetNavigationSystem()->GetMainNavData();
	query.bAllowPartialPaths = false;
	int32* nodes = 0;

	if (GetWorld()->GetNavigationSystem()->TestPathSync(query, EPathFindingMode::Hierarchical, nodes))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ATwoKitchenAIController::MoveToLineNode(ALineNode* TargetNode)
{
	ATwoKitchenAICharacter* character = Cast<ATwoKitchenAICharacter>(GetPawn());

	if (CheckIfMoveIsLegal(TargetNode->GetActorLocation()))
	{
		StateEnum = EAIEnum::AI_MOVINGTOLINENODE;
		GoalLocation = TargetNode->GetActorLocation();
		GoalLineNode = TargetNode;
		MoveToLocation(GoalLocation, 1.0f, false, true, false);
		CurrentLineNode = TargetNode;
	}
	else
	{
		StateEnum = EAIEnum::AI_IDLE;
		character->CantReachDestination();
	}
}
