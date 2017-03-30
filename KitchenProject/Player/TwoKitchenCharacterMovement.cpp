// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "GameFramework/GameNetworkManager.h"
#include "TwoKitchenCharacterMovement.h"

void UTwoKitchenCharacterMovement::ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration)
{
	check(CharacterOwner != NULL);

	// Can only start sending moves if our controllers are synced up over the network, otherwise we flood the reliable buffer.
	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
	if (PC && PC->AcknowledgedPawn != CharacterOwner)
	{
		return;
	}

	// Bail out if our character's controller doesn't have a Player. This may be the case when the local player
	// has switched to another controller, such as a debug camera controller.
	if (PC && PC->Player == nullptr)
	{
		return;
	}

	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	if (!ClientData)
	{
		return;
	}

	// Update our delta time for physics simulation.
	DeltaTime = ClientData->UpdateTimeStampAndDeltaTime(DeltaTime, *CharacterOwner, *this);

	// Find the oldest (unacknowledged) important move (OldMove).
	// Don't include the last move because it may be combined with the next new move.
	// A saved move is interesting if it differs significantly from the last acknowledged move
	FSavedMovePtr OldMove = NULL;
	if (ClientData->LastAckedMove.IsValid())
	{
		for (int32 i = 0; i < ClientData->SavedMoves.Num() - 1; i++)
		{
			const FSavedMovePtr& CurrentMove = ClientData->SavedMoves[i];
			if (CurrentMove->IsImportantMove(ClientData->LastAckedMove))
			{
				OldMove = CurrentMove;
				break;
			}
		}
	}

	// Get a SavedMove object to store the movement in.
	FSavedMovePtr NewMove = ClientData->CreateSavedMove();
	if (NewMove.IsValid() == false)
	{
		return;
	}

	NewMove->SetMoveFor(CharacterOwner, DeltaTime, NewAcceleration, *ClientData);

	// see if the two moves could be combined
	// do not combine moves which have different TimeStamps (before and after reset).
	if (ClientData->PendingMove.IsValid() && !ClientData->PendingMove->bOldTimeStampBeforeReset && ClientData->PendingMove->CanCombineWith(NewMove, CharacterOwner, ClientData->MaxResponseTime * CharacterOwner->GetWorldSettings()->GetEffectiveTimeDilation()))
	{
		//SCOPE_CYCLE_COUNTER(STAT_CharacterMovementCombineNetMove);

		// Only combine and move back to the start location if we don't move back in to a spot that would make us collide with something new.
		const FVector OldStartLocation = ClientData->PendingMove->GetRevertedLocation();
		if (!OverlapTest(OldStartLocation, ClientData->PendingMove->StartRotation.Quaternion(), UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CharacterOwner))
		{
			FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, EScopedUpdate::DeferredUpdates);
			UE_LOG(LogNetPlayerMovement, VeryVerbose, TEXT("CombineMove: add delta %f + %f and revert from %f %f to %f %f"), DeltaTime, ClientData->PendingMove->DeltaTime, UpdatedComponent->GetComponentLocation().X, UpdatedComponent->GetComponentLocation().Y, OldStartLocation.X, OldStartLocation.Y);

			// to combine move, first revert pawn position to PendingMove start position, before playing combined move on client
			const bool bNoCollisionCheck = true;
			UpdatedComponent->SetWorldLocationAndRotation(OldStartLocation, ClientData->PendingMove->StartRotation, false);
			Velocity = ClientData->PendingMove->StartVelocity;

			SetBase(ClientData->PendingMove->StartBase.Get(), ClientData->PendingMove->StartBoneName);
			CurrentFloor = ClientData->PendingMove->StartFloor;

			// Now that we have reverted to the old position, prepare a new move from that position,
			// using our current velocity, acceleration, and rotation, but applied over the combined time from the old and new move.

			NewMove->DeltaTime += ClientData->PendingMove->DeltaTime;

			if (PC)
			{
				// We reverted position to that at the start of the pending move (above), however some code paths expect rotation to be set correctly
				// before character movement occurs (via FaceRotation), so try that now. The bOrientRotationToMovement path happens later as part of PerformMovement() and PhysicsRotation().
				CharacterOwner->FaceRotation(PC->GetControlRotation(), NewMove->DeltaTime);
			}

			SaveBaseLocation();
			NewMove->SetInitialPosition(CharacterOwner);

			// Remove pending move from move list. It would have to be the last move on the list.
			if (ClientData->SavedMoves.Num() > 0 && ClientData->SavedMoves.Last() == ClientData->PendingMove)
			{
				ClientData->SavedMoves.Pop();
			}
			ClientData->FreeMove(ClientData->PendingMove);
			ClientData->PendingMove = NULL;
		}
		else
		{
			//UE_LOG(LogNet, Log, TEXT("Not combining move, would collide at start location"));
		}
	}

	// Acceleration should match what we send to the server, plus any other restrictions the server also enforces (see MoveAutonomous).
	Acceleration = NewMove->Acceleration.GetClampedToMaxSize(GetMaxAcceleration());
	AnalogInputModifier = ComputeAnalogInputModifier(); // recompute since acceleration may have changed.

														// Perform the move locally
	CharacterOwner->ClientRootMotionParams.Clear();
	PerformMovement(NewMove->DeltaTime);

	NewMove->PostUpdate(CharacterOwner, FSavedMove_Character::PostUpdate_Record);

	// Add NewMove to the list
	if (CharacterOwner->bReplicateMovement)
	{
		ClientData->SavedMoves.Push(NewMove);
		//const bool bCanDelayMove = (CVarNetEnableMoveCombining.GetValueOnGameThread() != 0) && CanDelaySendingMove(NewMove);

		/**
		if (bCanDelayMove && ClientData->PendingMove.IsValid() == false)
		{
			// Decide whether to hold off on move
			// send moves more frequently in small games where server isn't likely to be saturated

			float NetMoveDelta;
			UPlayer* Player = (PC ? PC->Player : NULL);

			if (Player && (Player->CurrentNetSpeed > 10000) && (GetWorld()->GameState != NULL) && (GetWorld()->GameState->PlayerArray.Num() <= 10))
			{
				NetMoveDelta = 0.011f;
			}
			else if (Player && CharacterOwner->GetWorldSettings()->GameNetworkManagerClass)
			{
				NetMoveDelta = FMath::Max(0.0222f, 2 * GetDefault<AGameNetworkManager>(CharacterOwner->GetWorldSettings()->GameNetworkManagerClass)->MoveRepSize / Player->CurrentNetSpeed);
			}
			else
			{
				NetMoveDelta = 0.011f;
			}


			if ((GetWorld()->TimeSeconds - ClientData->ClientUpdateTime) * CharacterOwner->GetWorldSettings()->GetEffectiveTimeDilation() < NetMoveDelta)
			{
				// Delay sending this move.
				ClientData->PendingMove = NewMove;
				return;
			}
		}
		**/

		ClientData->ClientUpdateTime = GetWorld()->TimeSeconds;

		// Send move to server if this character is replicating movement
		CallServerMove(NewMove.Get(), OldMove.Get());
	}

	ClientData->PendingMove = NULL;
}


