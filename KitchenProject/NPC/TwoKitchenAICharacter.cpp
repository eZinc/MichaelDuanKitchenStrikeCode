// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "../NPC/TwoKitchenAIController.h"
#include "../Kitchen/LineNode.h"
#include "../Kitchen/KitchenManager.h"
#include "../Player/TwoKitchenPlayerState.h"
#include "TwoKitchenAICharacter.h"


void ATwoKitchenAICharacter::ReachedDestination()
{
	if (HasAuthority())
	{
		NetworkedPlaySound(ReachedLocation, true);
	}
}

void ATwoKitchenAICharacter::CantReachDestination()
{
	if (HasAuthority())
	{
		NetworkedPlaySound(CantReachLocation, true);
	}
}

void ATwoKitchenAICharacter::Select(AKitchenProjectCharacter* Instigator)
{
	if (HasAuthority())
	{
		if (CanBossAround(Instigator))
		{
			if (isSelected)
			{
				//NetworkedPlaySound(AffirmativeWaiting, true);
				isSelected = false;
				Instigator->Selected(NULL);
			}
			else
			{
				NetworkedPlaySound(AffirmativeWaiting, true);
				isSelected = true;
				Instigator->Selected(this);
			}
		}
		else
			NetworkedPlaySound(NegativeNotBoss, true);
	}
}

void ATwoKitchenAICharacter::MoveToOrder(FVector Destination, AKitchenProjectCharacter* Instigator)
{
	if (HasAuthority() && CanBossAround(Instigator))
	{
		NetworkedPlaySound(AffirmativeGoing, true);

		ATwoKitchenAIController* c = Cast<ATwoKitchenAIController>(GetController());
		c->MoveToOrder(Destination);
	}
}

bool ATwoKitchenAICharacter::CanBossAround(AKitchenProjectCharacter* Instigator)
{
	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(Instigator->PlayerState);
	if (s)
	{
		int32 id = s->GetTeamID();
		if (id == PlayerIDCanOwn)
			return true;
	}
	return false;
}

FString ATwoKitchenAICharacter::GetLabel()
{
	if (isSelected)
	{
		return "Deselect " + GetName();
	}
	else
	{
		return "Select " + GetName();
	}
}

FString ATwoKitchenAICharacter::GetName()
{
	return "Civilian";
}

void ATwoKitchenAICharacter::UseObject(AActor* Instigator)
{
	if (Instigator->IsA(AKitchenProjectCharacter::StaticClass()))
	{
		AKitchenProjectCharacter* c = Cast < AKitchenProjectCharacter>(Instigator);
		if (c)
		{
			FVector v = c->GetActorLocation() - GetActorLocation();
			float distance = v.Size();

			if (distance <= c->ShoveDistance)
			{
				NetworkedPlaySound(ShoveSound, false);
				GetShoved(c);
			}
			else
			{
				Select(c);
			}
		}
	}
}

void ATwoKitchenAICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATwoKitchenAICharacter, isSelected);
	DOREPLIFETIME(ATwoKitchenAICharacter, PlayerIDCanOwn);
	DOREPLIFETIME(ATwoKitchenAICharacter, KitchenOwner);

}

void ATwoKitchenAICharacter::SelectFail(AKitchenProjectCharacter* own)
{
	NetworkedPlaySound(NegativeNotBoss, true);
}

void ATwoKitchenAICharacter::ServerSelectMinion_Implementation(AKitchenProjectCharacter* Instigator)
{
	if (HasAuthority())
	{
		Select(Instigator);
	}
}

bool ATwoKitchenAICharacter::ServerSelectMinion_Validate(AKitchenProjectCharacter* own)
{
	return true;
}

void ATwoKitchenAICharacter::GoToLineNode(class ALineNode* TargetNode)
{
	ATwoKitchenAIController* con = Cast<ATwoKitchenAIController>(GetController());
	con->MoveToLineNode(TargetNode);
}

void ATwoKitchenAICharacter::Die(AActor* Killer, FVector LaunchVector, FVector HitLocation)
{
	// If in line, notify that the line can move up
	if(CurrentKitchen)
		CurrentKitchen->CustomerKilled(this);

	GetController()->Destroy();

	Super::Die(Killer, LaunchVector, HitLocation);


}