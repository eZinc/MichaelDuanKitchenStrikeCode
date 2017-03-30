// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "Net/UnrealNetwork.h"
#include "TwoKitchenPlayerState.h"
#include "CustomSpectatorPawn.h"
#include "../KitchenProjectCharacter.h"
#include "../Kitchen/DeliveryManager.h"
#include "TwoKitchenGameMode.h"
#include "../TwoKitchenGamemode.h"
#include "../TwoKitchenGamestate.h"
#include "../Kitchen/Computer.h"
#include "TwoKitchenPlayerController.h"

void ATwoKitchenPlayerController::PawnLeavingGame()
{
	Super::PawnLeavingGame();

	ATwoKitchenGamemode* g = Cast<ATwoKitchenGamemode>(GetWorld()->GetAuthGameMode());
	ATwoKitchenGamestate* GameState = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());
	ATwoKitchenPlayerState* ps = Cast<ATwoKitchenPlayerState>(PlayerState);

	ps->GetKitchenManager()->RemovePlayer(this);

	if (ps->GetTeamID() == 0 || ps->GetTeamID() == 1)
	{
		// check if any kitchen managers are completely empty and if so end the game by forfeit
		if (ps->GetKitchenManager()->PlayerArray.Num() == 0)
		{
			g->EndGame(ps->TeamID);
		}
	}
	else
	{
		GameState->RemoveSpectator(ps);
	}

	g->SendConsoleMessage(ps->GetDisplayName() + " has disconnected.");
	g->TellClientsRefreshScoreboard();
}

void ATwoKitchenPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	bReplicates = true;
}

void ATwoKitchenPlayerController::DrawClientHUD(APawn* InPawn)
{
	if (InPawn->IsA(ACustomSpectatorPawn::StaticClass()))
	{
		DrawSpectatorHUD();
	}
	else if (InPawn->IsA(AKitchenProjectCharacter::StaticClass()))
	{
		DrawPlayerHUD();
	}
}

void ATwoKitchenPlayerController::GiveMessage(FString ChatMessage)
{
	SendServerChatMessage(ChatMessage);
}

void ATwoKitchenPlayerController::ready()
{
	ServerRequestReady();
}

void ATwoKitchenPlayerController::ServerRequestReady_Implementation()
{
	ATwoKitchenGamemode* g = Cast<ATwoKitchenGamemode>(GetWorld()->GetAuthGameMode());
	g->PlayerReady(this);
}

bool ATwoKitchenPlayerController::ServerRequestReady_Validate()
{
	return true;
}

void ATwoKitchenPlayerController::shopname(FString YourShopName)
{
	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(PlayerState);
	if (s->TeamID == -1)
	{
		TakeMessage("CONSOLE", "Game", "You don't have a shop to name!");
		return;
	}

	if (YourShopName.Len() > 20)
	{
		TakeMessage("CONSOLE", "Game", "Shop name exceeded 20 char limit.");
		return;
	}

	ServerUpdateShopName(YourShopName);
}

void ATwoKitchenPlayerController::name(FString YourPlayerName)
{
	if (YourPlayerName.Len() > 18)
	{
		TakeMessage("CONSOLE", "Game", "Player name exceeded 18 char limit.");
		return;
	}

	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(PlayerState);
	s->SetDisplayName(YourPlayerName);
	ServerUpdatePlayerName(YourPlayerName);
}

void ATwoKitchenPlayerController::ServerUpdateShopName_Implementation(const FString& ShopName)
{
	if (ShopName.Len() > 20)
		return;

	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(PlayerState);
	s->GetKitchenManager()->SetShopName(ShopName);
	ATwoKitchenGamemode* g = Cast<ATwoKitchenGamemode>(GetWorld()->GetAuthGameMode());
	if (s->GetTeamID() == 0)
	{
		g->SendConsoleMessage("Blue team's shop name changed to " + ShopName + "!");
	}
	else if(s->GetTeamID() == 1)
	{
		g->SendConsoleMessage("Red team's shop name changed to " + ShopName + "!");
	}
}

bool ATwoKitchenPlayerController::ServerUpdateShopName_Validate(const FString& ShopName)
{
	return true;
}

// Serverside change name
void ATwoKitchenPlayerController::ServerUpdatePlayerName_Implementation(const FString& PlayerName)
{
	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(PlayerState);
	s->SetDisplayName(PlayerName);
	if (s->TeamID == -1)
	{
		ATwoKitchenGamemode* g = Cast<ATwoKitchenGamemode>(GetWorld()->GetAuthGameMode());
		g->TellClientsRefreshScoreboard();
	}
}

bool ATwoKitchenPlayerController::ServerUpdatePlayerName_Validate(const FString& PlayerName)
{
	return true;
}

void ATwoKitchenPlayerController::SendServerChatMessage_Implementation(const FString& ChatMessage)
{
	if (HasAuthority())
	{
		ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(PlayerState);
		FString PlayerName = s->GetDisplayName();
		int32 TeamID = s->GetTeamID();
		FString PlayerTeam = "";
		if (TeamID == 0)
		{
			PlayerTeam = "Blue";
		}
		else if (TeamID == 1)
		{
			PlayerTeam = "Red";
		}
		else
		{
			PlayerTeam = "Spectator";
		}

		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* p = *Iterator;
			ATwoKitchenPlayerController* k = Cast<ATwoKitchenPlayerController>(p);
			k->SendAllClientsMessage(PlayerName, PlayerTeam, ChatMessage);
		}
	}
}

bool ATwoKitchenPlayerController::SendServerChatMessage_Validate(const FString& ChatMessage)
{
	return true;
}

void ATwoKitchenPlayerController::SendAllClientsMessage_Implementation(const FString& PlayerName, const FString& PlayerTeam, const FString& ChatMessage)
{
	TakeMessage(PlayerName, PlayerTeam, ChatMessage);
}

void ATwoKitchenPlayerController::ServerNotifyGameStarted_Implementation()
{
	ClientNotifyGameStarted();
}

void ATwoKitchenPlayerController::ServerNotifyGameEnded_Implementation(const FString& Winner, int32 WinnerTeam, int32 PlayerOneMoney, int32 PlayerTwoMoney, int32 PlayerOneRent, int32 PlayerTwoRent,
	int32 PlayerOneTotal, int32 PlayerTwoTotal, int32 PlayerOneOldStrikes, int32 PlayerTwoOldStrikes,
	int32 PlayerOneUpdatedStrikes, int32 PlayerTwoUpdatedStrikes)
{
	ClientNotifyGameEnded(Winner, WinnerTeam, PlayerOneMoney, PlayerTwoMoney, PlayerOneRent, PlayerTwoRent,
		PlayerOneTotal, PlayerTwoTotal, PlayerOneOldStrikes, PlayerTwoOldStrikes,
		PlayerOneUpdatedStrikes, PlayerTwoUpdatedStrikes);
}

void ATwoKitchenPlayerController::ServerNotifyDayEnded_Implementation(int32 PlayerOneMoney, int32 PlayerTwoMoney, int32 PlayerOneRent, int32 PlayerTwoRent,
	int32 PlayerOneTotal, int32 PlayerTwoTotal, int32 PlayerOneOldStrikes, int32 PlayerTwoOldStrikes,
	int32 PlayerOneUpdatedStrikes, int32 PlayerTwoUpdatedStrikes)
{
	ClientNotifyDayEnded(PlayerOneMoney, PlayerTwoMoney, PlayerOneRent, PlayerTwoRent,
		PlayerOneTotal, PlayerTwoTotal, PlayerOneOldStrikes, PlayerTwoOldStrikes,
		PlayerOneUpdatedStrikes, PlayerTwoUpdatedStrikes);
}

void ATwoKitchenPlayerController::AllowedUseComputer_Implementation()
{
	ClientDrawComputerHUD();
}

void ATwoKitchenPlayerController::ServerExitedComputer_Implementation()
{
	ComputerInUse->bIsInUse = false;
	ComputerInUse->MulticastShowUsage(ComputerInUse->bIsInUse);
	ComputerInUse = NULL;
}

bool ATwoKitchenPlayerController::ServerExitedComputer_Validate()
{
	return true;
}

void ATwoKitchenPlayerController::ServerDrawPregameHUD_Implementation()
{
	ClientDrawPregameHUD();
}

void ATwoKitchenPlayerController::ServerDrawSpectatorHUD_Implementation()
{
	ClientDrawSpectatorHUD();
}

void ATwoKitchenPlayerController::ServerDrawPlayerHUD_Implementation()
{
	ClientDrawPlayerHUD();
}

void ATwoKitchenPlayerController::RequestTeam(int32 team)
{
	ServerRequestTeam(team);
}


void ATwoKitchenPlayerController::ServerRequestTeam_Implementation(int32 team)
{
	if (HasAuthority())
	{
		ATwoKitchenGamemode* g = Cast<ATwoKitchenGamemode>(GetWorld()->GetAuthGameMode());
		g->JoinTeam(this, team);
	}
}

void ATwoKitchenPlayerController::RequestChangePassword(const FString& password)
{
	ServerRequestChangePassword(password);
}

void ATwoKitchenPlayerController::ServerRequestChangePassword_Implementation(const FString& password)
{
	ComputerInUse->ChangePassword(password);
}

bool ATwoKitchenPlayerController::ServerRequestChangePassword_Validate(const FString& password)
{
	return true;
}

void ATwoKitchenPlayerController::RequestComputerLogin(const FString& password)
{
	ServerRequestComputerLogin(password);
}

void ATwoKitchenPlayerController::ServerRequestComputerLogin_Implementation(const FString& password)
{
	ATwoKitchenPlayerState* ps = Cast<ATwoKitchenPlayerState>(PlayerState);

	if (ps)
	{
		if (password == ComputerInUse->Password)
		{
			AllowComputerLogin(true);
		}
		else
		{
			AllowComputerLogin(false);
		}
	}
}

void ATwoKitchenPlayerController::AllowComputerLogin_Implementation(bool ShouldUnlock)
{
	ComputerUnlock(ShouldUnlock);
}


bool ATwoKitchenPlayerController::ServerRequestComputerLogin_Validate(const FString& password)
{
	return true;
}

bool ATwoKitchenPlayerController::ServerRequestTeam_Validate(int32 team)
{
	return true;
}

void ATwoKitchenPlayerController::ServerRefreshScoreboard_Implementation()
{
	ClientRefreshScoreboard();
}

void ATwoKitchenPlayerController::ClientExitedComputer()
{
	ServerExitedComputer();
}

void ATwoKitchenPlayerController::UpdateWallet_Implementation(int32 amount, int32 total, const FString& Reason)
{
	UpdateWalletGUI(amount, total, Reason);
}

void ATwoKitchenPlayerController::OrderItem(FName Name, UClass* ItemClass, int32 quantity, int32 price, int32 MinutesToDeliver)
{
	ServerOrderItem(Name, ItemClass, quantity, price, MinutesToDeliver);
}

void ATwoKitchenPlayerController::ServerOrderItem_Implementation(FName Name, UClass* ItemClass, int32 quantity, int32 price, int32 MinutesToDeliver)
{
	if (ComputerInUse)
	{
		ComputerInUse->KitchenManager->DeliveryManager->ServerOrderItem(Name, ItemClass, quantity, price, MinutesToDeliver);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Your message"));

	}
}

bool ATwoKitchenPlayerController::ServerOrderItem_Validate(FName Name, UClass* ItemClass, int32 quantity, int32 price, int32 MinutesToDeliver)
{
	return true;
}