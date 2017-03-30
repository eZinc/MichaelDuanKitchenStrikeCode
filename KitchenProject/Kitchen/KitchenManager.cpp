// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "../NPC/TwoKitchenAIController.h"
#include "CustomerSpawnerComponent.h"
#include "../TwoKitchenGamemode.h"
#include "../Player/TwoKitchenPlayerController.h"
#include "../Player/TwoKitchenPlayerState.h"
#include "LineNode.h"
#include "KitchenManager.h"


// Sets default values
AKitchenManager::AKitchenManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	CustomerControllers.SetNumUninitialized(0);
	PlayerStateArray.SetNumUninitialized(0);
	PlayerControllerArray.SetNumUninitialized(0);
	ShopSigns.SetNumUninitialized(0);

	CustomerSpawner = CreateDefaultSubobject<UCustomerSpawnerComponent>(TEXT("CustomerSpawner"), false);
	CustomerSpawner->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AKitchenManager::BeginPlay()
{
	Super::BeginPlay();
	KitchenRating = DefaultKitchenRating;
	CustomerSpawner->Initialize(this);
}

// Called every frame
void AKitchenManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AKitchenManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKitchenManager, Money);
	DOREPLIFETIME(AKitchenManager, ShopName);
	DOREPLIFETIME(AKitchenManager, ShopSigns);
	DOREPLIFETIME(AKitchenManager, Rent);
	DOREPLIFETIME(AKitchenManager, Strikes);
	DOREPLIFETIME(AKitchenManager, TeamID);
	DOREPLIFETIME(AKitchenManager, PlayerStateArray);
	DOREPLIFETIME(AKitchenManager, KitchenRating);
}

void AKitchenManager::UpdateShopSign_Implementation(const FString& ShopNamea)
{
	for (auto i : ShopSigns)
	{
		i->GetTextRender()->SetText(ShopNamea);
	}
}

bool AKitchenManager::AcceptCustomer(ATwoKitchenAIController* Customer)
{
	if (IsKitchenFull())
	{
		return false;
	}


	CustomerControllers.Add(Customer);
	MoveToBestSpotInLine(Customer, StreetLine);
	return true;
}

bool AKitchenManager::IsKitchenFull()
{
	ALineNode* FirstNode = (StreetLine[StreetLine.Num() - 1]);
	if (FirstNode->IsOccupied)
	{
		return true;
	}
	return false;
}

void AKitchenManager::AddMoney(const FString& Reason, int32 m)
{
	Money += m;

	for (auto i : PlayerControllerArray)
	{
		i->UpdateWallet(m, Money, Reason);
	}
}


void AKitchenManager::KickCustomer(ATwoKitchenAIController* Customer, ALineNode* Node)
{
	// add more for them leaving and going out the street
	CustomerControllers.Remove(Customer);
	Customer->MoveToLineNode(ExitDoor);

	if (CashierOneLine.Contains(Node))
		MoveLineUp(CashierOneLine);
	else if (CashierTwoLine.Contains(Node))
		MoveLineUp(CashierTwoLine);
	else if (CashierThreeLine.Contains(Node))
		MoveLineUp(CashierThreeLine);
}

void AKitchenManager::MoveToBestSpotInLine(ATwoKitchenAIController* Customer, TArray<class ALineNode*> Line)
{
	for (int i = 0; i < Line.Num(); i++)
	{
		if (!Line[i]->IsOccupied)
		{
			if (Line == StreetLine)
			{
				if (i == 0)
				{
					MoveToBestCashierLine(Customer);
					return;
				}
			}
			Customer->MoveToLineNode(Line[i]);
			Line[i]->Resident = Customer->GetPawn();
			Line[i]->IsOccupied = true;
			return;
		}
	}
}

void AKitchenManager::MoveLineUp(TArray<class ALineNode*> LineToMove)
{
	for (int i = 1; i < LineToMove.Num(); i++)
	{
		if (LineToMove[i]->Resident)
		{
			ATwoKitchenAIController* Customer = Cast<ATwoKitchenAIController>(LineToMove[i]->Resident->GetController());
			if (Customer)
			{
				Customer->MoveToLineNode(LineToMove[i - 1]);
			}
		}
		LineToMove[i-1]->Resident = LineToMove[i]->Resident;
		LineToMove[i-1]->IsOccupied = LineToMove[i]->IsOccupied;
	}

	LineToMove[LineToMove.Num() - 1]->Resident = NULL;
	LineToMove[LineToMove.Num() - 1]->IsOccupied = false;
	
	if (StreetLine[0]->IsOccupied)
	{
		ATwoKitchenAIController* contr = Cast<ATwoKitchenAIController>(StreetLine[0]->Resident->GetController());
		MoveToBestCashierLine(contr);
	}
}


void AKitchenManager::MoveToBestCashierLine(ATwoKitchenAIController* Customer)
{
	int32 CashOne = 0;
	int32 CashTwo = 0;
	int32 CashThree = 0;

	bool Full = true;

	for (int i = 0; i < CashierOneLine.Num(); i++)
	{
		if (CashierOneLine[i]->IsOccupied)
		{
			CashOne++;
		}
		else
			Full = false;
	}

	for (int i = 0; i < CashierTwoLine.Num(); i++)
	{
		if (CashierTwoLine[i]->IsOccupied)
		{
			CashTwo++;
		}
		else
			Full = false;
	}

	for (int i = 0; i < CashierThreeLine.Num(); i++)
	{
		if (CashierThreeLine[i]->IsOccupied)
		{
			CashThree++;
		}
		else
			Full = false;
	}

	if (Full == true)
	{
		Customer->MoveToLineNode(StreetLine[0]);
		StreetLine[0]->Resident = Customer->GetPawn();
		StreetLine[0]->IsOccupied = true;
		return;
	}


	// if the cashier lines are all full
	int32 smallest = FMath::Min3(CashOne, CashTwo, CashThree);
	
	if (smallest == CashOne)
	{
		MoveToBestSpotInLine(Customer, CashierOneLine);
	}
	else if (smallest == CashTwo)
	{
		MoveToBestSpotInLine(Customer, CashierTwoLine);
	}
	else if (smallest == CashThree)
	{
		MoveToBestSpotInLine(Customer, CashierThreeLine);
	}
	MoveLineUp(StreetLine);

}

// Accept a player maintaining 0 1 2 order
bool AKitchenManager::AcceptPlayer(ATwoKitchenPlayerController* contr)
{
	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(contr->PlayerState);

	ATwoKitchenGamemode* g = Cast<ATwoKitchenGamemode>(GetWorld()->GetAuthGameMode());

	if (PlayerStateArray.Num() == 3)
	{
		return false;
	}

	PlayerStateArray.Add(s);
	PlayerControllerArray.Add(contr);
	s->SetKitchenManager(this);
	return true;
}

// Remove a player but maintain 0 1 2 order
void AKitchenManager::RemovePlayer(ATwoKitchenPlayerController* contr)
{
	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(contr->PlayerState);

	PlayerStateArray.Remove(s);
	PlayerControllerArray.Remove(contr);

	TArray<ATwoKitchenPlayerState*> TempArray;
	TempArray.SetNumUninitialized(0);

	for (int i = 0; i < PlayerStateArray.Num(); i++) 
	{
		if (PlayerStateArray[i] != NULL)
		{
			TempArray.Add(PlayerStateArray[i]);
		}
	}

	PlayerStateArray = TempArray;
}

void AKitchenManager::MulticastKitchenRatingUpdate_Implementation(float Change, float Total)
{
	KitchenRatingUpdate(Change, Total);
}

bool AKitchenManager::MulticastKitchenRatingUpdate_Validate(float Change, float Total)
{
	return true;
}

void AKitchenManager::CustomerKilled(ATwoKitchenAICharacter* Victim)
{
	ATwoKitchenAIController* cont = Cast<ATwoKitchenAIController>(Victim->GetController());

	CustomerControllers.Remove(cont);

	AddKitchenRating(CustomerKilledPenalty);

	// Search for the line that corresponds to the victim
	ALineNode* Current = cont->CurrentLineNode;

	if (StreetLine.Contains(Current))
	{
		// All we have to do is move those behind up
		MoveLineStartingAt(StreetLine, Current);
	}
	else
	{
		// Determine which line should be moved up
		TArray<class ALineNode*> Line;
		if (CashierOneLine.Contains(Current))
			Line = CashierOneLine;
		else if (CashierTwoLine.Contains(Current))
			Line = CashierTwoLine;
		else if (CashierThreeLine.Contains(Current))
			Line = CashierThreeLine;
		else
			return;

		// Move up in the line itself
		MoveLineStartingAt(Line, Current);

		// Move up the street line
		if (StreetLine[0]->Resident)
		{
			ATwoKitchenAIController* controller = Cast<ATwoKitchenAIController>(StreetLine[0]->Resident->GetController());
			MoveToBestCashierLine(controller);
		}
	}
}

void AKitchenManager::MoveLineStartingAt(TArray<class ALineNode*> Line, class ALineNode* Target)
{
	int32 index = Line.Find(Target);
	for (int i = index; i < Line.Num() - 1; i++)
	{
		// Copy the contents from the line node behind it
		Line[i]->Resident = Line[i + 1]->Resident;
		Line[i]->IsOccupied = Line[i + 1]->IsOccupied;
		
		// Tell the pawn to move there
		if (Line[i]->Resident)
		{
			ATwoKitchenAIController* Customer = Cast<ATwoKitchenAIController>(Line[i]->Resident->GetController());
			Customer->MoveToLineNode(Line[i]);
		}
	}
	// The very last thing in line is guaranteed to be empty
	Line[Line.Num() - 1]->Resident = NULL;
	Line[Line.Num() - 1]->IsOccupied = false;
}