// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "KitchenProject.h"
#include "GameFramework/PlayerState.h"
#include "TwoKitchenGamemode.h"
#include "Kitchen/CustomerSpawnerComponent.h"
#include "Player/TwoKitchenPlayerState.h"
#include "TwoKitchenGamestate.h"
#include "Engine/TargetPoint.h"
#include "NPC/SimpleCarSpawner.h"
#include "Kitchen/KitchenManager.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Player/CustomSpectatorPawn.h"
#include "KitchenProject/Player/TwoKitchenPlayerController.h"
#include "KitchenProjectCharacter.h"

ATwoKitchenGamemode::ATwoKitchenGamemode()
{
	CarSpawners.SetNumUninitialized(0);
}


// TODO: Use separate classes as opposed to a huge glob of functions
void ATwoKitchenGamemode::BeginPlay()
{
	Super::BeginPlay();

	ATwoKitchenGamestate* GameState = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());

	for (TActorIterator<AKitchenManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Find the spawn points for the two players
		AKitchenManager* temp = Cast<AKitchenManager>(*ActorItr);

		if (temp->GetTeamID() == 0)
		{
			GameState->TeamOneKitchen = temp;
			temp->SetShopName("BLUE'S BURGERS");
		}
		else if (temp->GetTeamID() == 1)
		{
			GameState->TeamTwoKitchen = temp;
			temp->SetShopName("RED'S BURGERS");
		}
	}

	for (TActorIterator<ASimpleCarSpawner> CarSpawnerItr(GetWorld()); CarSpawnerItr; ++CarSpawnerItr)
	{
		ASimpleCarSpawner* temp = Cast<ASimpleCarSpawner>(*CarSpawnerItr);
		CarSpawners.Add(temp);
	}
}

void ATwoKitchenGamemode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

}

void ATwoKitchenGamemode::StartGame()
{
	ATwoKitchenGamestate* GameState = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());

	if (GameState->TeamOneKitchen == NULL || GameState->TeamTwoKitchen == NULL)
	{
		SendConsoleMessage("You must have at least one player per team!");
		return;
	}

	// Start up the customer spawners
	GameState->TeamOneKitchen->CustomerSpawner->StartSpawning();
	GameState->TeamTwoKitchen->CustomerSpawner->StartSpawning();

	GameState->bGameInProgress = true;

	GameState->MinutesPerSecond = (1440 / (MinutesPerDay * 60));
	GameState->minute = 0;
	GameState->day = 1;

	GameState->TeamOneKitchen->SetRent(DefaultRentRate);
	GameState->TeamTwoKitchen->SetRent(DefaultRentRate);

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* p = *Iterator;
		ATwoKitchenPlayerController* k = Cast<ATwoKitchenPlayerController>(p);
		k->ServerNotifyGameStarted();
	}

	// Start up car spawners
	for (TActorIterator<ASimpleCarSpawner> CarSpawnerItr(GetWorld()); CarSpawnerItr; ++CarSpawnerItr)
	{
		// Find the spawn points for the two players
		ASimpleCarSpawner* temp = Cast<ASimpleCarSpawner>(*CarSpawnerItr);
		temp->StartSpawningCars();
	}

	SendConsoleMessage("GAME HAS STARTED. Good luck!");
}

void ATwoKitchenGamemode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ATwoKitchenGamestate* GameState = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());
	if (GameState->bGameInProgress)
	{
		AccurateMinutes += GameState->MinutesPerSecond * DeltaTime;
		if (AccurateMinutes > 1440)
		{
			IncrementDay();
		}

		if ((int)AccurateMinutes != GameState->minute)
		{
			GameState->minute = (int)AccurateMinutes;
		}
	}
}

void ATwoKitchenGamemode::IncrementDay()
{
	ATwoKitchenGamestate* GameState = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());

	// Give the leftover minutes to the next day
	float leftover = AccurateMinutes - 1440;
	AccurateMinutes = 0 + leftover;

	// Increment the day counter
	GameState->day += 1;

	PlayerOneOldStrikes = GameState->TeamOneKitchen->GetStrikes();
	PlayerTwoOldStrikes = GameState->TeamTwoKitchen->GetStrikes();

	// Deduct rent per day from wallets
	// Player One
	CollectRent(GameState->TeamOneKitchen);

	// Player Two
	CollectRent(GameState->TeamTwoKitchen);


	int32 PlayerOneUpdatedStrikes = GameState->TeamOneKitchen->GetStrikes();
	int32 PlayerTwoUpdatedStrikes = GameState->TeamTwoKitchen->GetStrikes();


	// Check for a loss
	if (PlayerOneUpdatedStrikes == StrikeLimit || PlayerTwoUpdatedStrikes == StrikeLimit)
	{
		EndGame(-1);
		return;
	}

	// This won't run if the game has actually ended.

	AKitchenManager* a = GameState->TeamOneKitchen;
	AKitchenManager* b = GameState->TeamTwoKitchen;

	// Game ended, so pass this information on to every player
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* p = *Iterator;
		ATwoKitchenPlayerController* k = Cast<ATwoKitchenPlayerController>(p);

		k->ServerNotifyDayEnded(a->GetMoney() + a->GetRent(), b->GetMoney() + b->GetRent(), a->GetRent(), b->GetRent(),
			a->GetMoney(), b->GetMoney(), PlayerOneOldStrikes, PlayerTwoOldStrikes,
			a->GetStrikes(), b->GetStrikes());
	}

	GameState->TeamOneKitchen->AddRent(RentIncreasePerDay);
	GameState->TeamTwoKitchen->AddRent(RentIncreasePerDay);
}

void ATwoKitchenGamemode::SendConsoleMessage(FString message)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* p = *Iterator;
		ATwoKitchenPlayerController* k = Cast<ATwoKitchenPlayerController>(p);
		k->SendAllClientsMessage("CONSOLE", "Game", message);
	}
}

// End the game
void ATwoKitchenGamemode::EndGame(int32 i)
{
	ATwoKitchenGamestate* GameState = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());

	if (GameState->TeamOneKitchen == NULL || GameState->TeamTwoKitchen == NULL)
		return;

	GameState->bGameInProgress = false;

	// Stop car spawners
	for (TActorIterator<ASimpleCarSpawner> CarSpawnerItr(GetWorld()); CarSpawnerItr; ++CarSpawnerItr)
	{
		// Find the spawn points for the two players
		ASimpleCarSpawner* temp = Cast<ASimpleCarSpawner>(*CarSpawnerItr);
		temp->StopSpawningCars();
	}

	// Stop customer spawners
	GameState->TeamOneKitchen->CustomerSpawner->StopSpawning();
	GameState->TeamTwoKitchen->CustomerSpawner->StopSpawning();

	FString WinnerName = "";
	int32 WinnerTeam = -1;

	// Player left/forfeit prematurely
	if (i != -1)
	{
		// BLUE left
		if (i == 0)
		{
			WinnerName = GameState->TeamTwoKitchen->GetShopName();
			WinnerTeam = 1;
			SendConsoleMessage(GameState->TeamTwoKitchen->GetShopName() + " WINS due to forfeit!");
		}
		// RED left
		else if (i == 1)
		{
			WinnerName = GameState->TeamOneKitchen->GetShopName();
			WinnerTeam = 0;
			SendConsoleMessage(GameState->TeamOneKitchen->GetShopName() + " WINS due to forfeit!");
		}

		return;
	}
	else
	{
		int32 PlayerOneWallet = GameState->TeamOneKitchen->GetMoney();
		int32 PlayerTwoWallet = GameState->TeamTwoKitchen->GetMoney();

		if (PlayerOneWallet == PlayerTwoWallet)
		{
			SendConsoleMessage("DRAW! Nobody wins.");
			WinnerName = "NOBODY ";
			WinnerTeam = -1;
		}
		else if (PlayerOneWallet > PlayerTwoWallet)
		{
			SendConsoleMessage("Blue team " + GameState->TeamOneKitchen->GetShopName() + " WINS!");
			WinnerName = GameState->TeamOneKitchen->GetShopName();
			WinnerTeam = 0;
		}
		else
		{
			SendConsoleMessage("Red team " + GameState->TeamTwoKitchen->GetShopName() + " WINS!");
			WinnerName = GameState->TeamTwoKitchen->GetShopName();
			WinnerTeam = 1;
		}
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AKitchenManager* a = GameState->TeamOneKitchen;
		AKitchenManager* b = GameState->TeamTwoKitchen;

		APlayerController* p = *Iterator;
		ATwoKitchenPlayerController* k = Cast<ATwoKitchenPlayerController>(p);
		k->ServerNotifyGameEnded(WinnerName, WinnerTeam, a->GetMoney() + a->GetRent(), b->GetMoney() + b->GetRent(), a->GetRent(), b->GetRent(),
			a->GetMoney(), b->GetMoney(), PlayerOneOldStrikes, PlayerTwoOldStrikes,
			a->GetStrikes(), b->GetStrikes());
	}
}

void ATwoKitchenGamemode::CollectRent(AKitchenManager* Kitchen)
{
	int32 rent = Kitchen->GetRent();
	Kitchen->AddMoney("Paid rent", -rent);

	// If the player fails to meet rent
	if (Kitchen->GetMoney() < 0)
	{
		// Add one strike
		Kitchen->AddStrikes(1);
	}
}

void ATwoKitchenGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ATwoKitchenGamemode* g = Cast<ATwoKitchenGamemode>(GetWorld()->GetAuthGameMode());
	ATwoKitchenPlayerController* c = Cast<ATwoKitchenPlayerController>(NewPlayer);
	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(NewPlayer->PlayerState);
	s->PlayerController = c;

	c->ServerDrawPregameHUD();
	SendConsoleMessage(s->GetDisplayName() + " has joined the game!");
	//g->AssignOpenSlot(c);
}

void ATwoKitchenGamemode::JoinTeam(class ATwoKitchenPlayerController* p, int32 team)
{
	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(p->PlayerState);
	ATwoKitchenGamestate* g = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());

	// remove for more teams
	if (team < -1 || team > 1)
		return;

	// Do checks if you can actually join each team
	if (team == -1)
	{
		// Join spectator
		s->SetTeamID(team);
	}
	else if (team == 0)
	{
		// Join Blue team
		s->SetTeamID(team);
		if (g->TeamOneKitchen->AcceptPlayer(p) == false)
			return;
	}
	else if (team == 1)
	{
		// Join Red team
		s->SetTeamID(team);
		if (g->TeamTwoKitchen->AcceptPlayer(p) == false)
			return;
	}

	CreatePlayer(s->PlayerController);
}


// deprecated, used to fill in slots that were open
int32 ATwoKitchenGamemode::AssignOpenSlot(ATwoKitchenPlayerController* p)
{
	if (cheapCount < 2)
	{
		ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(p->PlayerState);
		s->SetTeamID(cheapCount);
		if(cheapCount == 0)
			s->SetDisplayName("Blue");
		else
			s->SetDisplayName("Red");
		CreatePlayer(p);
		p->ServerDrawPlayerHUD();
	}
	else
	{
		p->ServerDrawSpectatorHUD();
	}
	cheapCount++;

	APlayerState* b = p->PlayerState;
	// do stuff with player state

	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(b);
	int32 ID = s->GetTeamID();
	for (TActorIterator<AKitchenManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Find the spawn points for the two players
		AKitchenManager* temp = Cast<AKitchenManager>(*ActorItr);
		FString IDString = FString::FromInt(ID);
		if (temp->ActorHasTag(FName(*IDString)))
		{
			s->SetKitchenManager(temp);
			break;
		}
	}

	return cheapCount;
}

int32 ATwoKitchenGamemode::RemoveFromSlot(ATwoKitchenPlayerController* p)
{
	return 0;
}

void ATwoKitchenGamemode::CreatePlayer(ATwoKitchenPlayerController* p)
{
	for (TActorIterator<ATargetPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Find the spawn points for the two players
		ATargetPoint* temp = Cast<ATargetPoint>(*ActorItr);
		if (temp->ActorHasTag(FName(TEXT("Player1Start"))))
		{
			PlayerOneSpawnPoint = temp;
		}
		else if (temp->ActorHasTag(FName(TEXT("Player2Start"))))
		{
			PlayerTwoSpawnPoint = temp;
		}
		else if (temp->ActorHasTag(FName(TEXT("SpectatorStart"))))
		{
			SpectatorSpawnPoint = temp;
		}
	}

	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(p->PlayerState);
	int32 ID = s->GetTeamID();
	FString f = FString::FromInt(ID);

	if (ID == 0)
	{
		// Spawn player one
		if (GetWorld())
		{
			if (IsValid(PlayerOneSpawnPoint))
			{
				FActorSpawnParameters params;
				params.Owner = p;
				AKitchenProjectCharacter* pawn = GetWorld()->SpawnActor<AKitchenProjectCharacter>(PlayerOneBlueprint, PlayerOneSpawnPoint->GetActorLocation(), PlayerOneSpawnPoint->GetActorRotation(), params);
				if (IsValid(pawn))
				{
					p->Possess(pawn);
				}
			}
		}
		p->ServerDrawPlayerHUD();
	}
	else if (ID == 1)
	{
		// Spawn player two
		if (GetWorld())
		{
			if (IsValid(PlayerTwoSpawnPoint))
			{
				FActorSpawnParameters params;
				params.Owner = p;
				AKitchenProjectCharacter* pawn = GetWorld()->SpawnActor<AKitchenProjectCharacter>(PlayerTwoBlueprint, PlayerTwoSpawnPoint->GetActorLocation(), PlayerTwoSpawnPoint->GetActorRotation(), params);
				if (IsValid(pawn))
				{
					p->Possess(pawn);
				}
			}
		}
		p->ServerDrawPlayerHUD();
	}
	else
	{
		if (GetWorld())
		{
			if (IsValid(SpectatorSpawnPoint))
			{
				FActorSpawnParameters params;
				params.Owner = p;
				ACustomSpectatorPawn* pawn = GetWorld()->SpawnActor<ACustomSpectatorPawn>(SpectatorBlueprint, SpectatorSpawnPoint->GetActorLocation(), SpectatorSpawnPoint->GetActorRotation(), params);
				if (IsValid(pawn))
				{
					p->Possess(pawn);
					ATwoKitchenGamestate* g = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());
					g->AddSpectator(Cast<ATwoKitchenPlayerState>(p->PlayerState));
				}
			}
		}
		p->ServerDrawSpectatorHUD();
	}

	TellClientsRefreshScoreboard();
}

void ATwoKitchenGamemode::SpawnHUD_Implementation(ATwoKitchenPlayerController* controller, APawn* InPawn)
{
	controller->DrawClientHUD(InPawn);
}

void ATwoKitchenGamemode::TellClientsRefreshScoreboard()
{
	// tell all the clients to refresh the scoreboard
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* p = *Iterator;
		ATwoKitchenPlayerController* k = Cast<ATwoKitchenPlayerController>(p);
		k->ServerRefreshScoreboard();
	}
}

void ATwoKitchenGamemode::PlayerReady(ATwoKitchenPlayerController* p)
{
	ATwoKitchenGamestate* GameState = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());

	if (GameState->bGameInProgress)
		return;

	ATwoKitchenPlayerState* sa = Cast<ATwoKitchenPlayerState>(p->PlayerState);

	if (sa->TeamID == -1)
		return;

	if (sa->IsReady == true)
	{
		SendConsoleMessage(sa->GetDisplayName() + " is NO LONGER READY!");
		sa->IsReady = false;
		return;
	}

	sa->IsReady = true;
	SendConsoleMessage(sa->GetDisplayName() + " is READY!");


	bool TeamOneHas = false;
	bool TeamTwoHas = false;
	// Check if all other players are ready
	// Check if there is at least one player on each team

	AKitchenManager* TeamOne = GameState->TeamOneKitchen;
	AKitchenManager* TeamTwo = GameState->TeamTwoKitchen;

	TeamOneHas = CheckIfTeamReady(TeamOne->GetPlayerStateArray());
	TeamTwoHas = CheckIfTeamReady(TeamTwo->GetPlayerStateArray());

	if (TeamOneHas && TeamTwoHas)
		StartGame();
	else
		SendConsoleMessage("Need at least one player on each team to start!");
}

// Check if given array of player states are ready
bool ATwoKitchenGamemode::CheckIfTeamReady(TArray<class ATwoKitchenPlayerState*> a)
{
	// If no player states are in the team then of course no
	if (a.Num() == 0)
		return false;
	
	for (int i = 0; i < a.Num(); i++)
	{
		ATwoKitchenPlayerState* s = a[i];
		if (s->GetIsReady() == false)
			return false;
	}

	return true;
}

void ATwoKitchenGamemode::RespawnPlayer(class ATwoKitchenPlayerController* p)
{
	// Maybe have different spawn points
	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(p->PlayerState);
	CreatePlayer(p);
}
