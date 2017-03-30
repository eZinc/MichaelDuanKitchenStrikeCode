// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "KitchenProject.h"
#include "Runtime/Engine/Classes/Engine/TextRenderActor.h"
#include "Player/TwoKitchenPlayerController.h"
#include "Kitchen/KitchenManager.h"
#include "UsableInterface.h"
#include "Player/TwoKitchenPlayerState.h"
#include "NPC/TwoKitchenAICharacter.h"
#include "NPC/TwoKitchenAIController.h"
#include "TwoKitchenGamemode.h"
#include "TwoKitchenGamestate.h"
#include "Player/TwoKitchenCharacterMovement.h"
#include "Weapons/FirstPersonWeapon.h"
#include "Kitchen/Computer.h"
#include "Player/WeaponInventoryComponent.h"
#include "UnrealNetwork.h"
#include "KitchenProjectCharacter.h"

//////////////////////////////////////////////////////////////////////////
// ATaleTwoKitchensCharacter

AKitchenProjectCharacter::AKitchenProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	MoveToMarker = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Marker"), false);
	MoveToMarker->AttachTo(GetCapsuleComponent());
	MoveToMarker->SetVisibility(true, true);

	CharacterVoiceComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("VoiceComponent"));
	CharacterVoiceComponent->AttachTo(GetCapsuleComponent());

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a follow camera
	FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FPCamera->AttachTo(GetCapsuleComponent()); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FPCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmsMesh"), false);
	ArmsMesh->AttachTo(FPCamera);
	ArmsMesh->SetVisibility(true, true);

	WeaponInventoryComponent = CreateDefaultSubobject<UWeaponInventoryComponent>(TEXT("WeaponInventoryComponent"));
	WeaponInventoryComponent->SetIsReplicated(true);

											  // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
											  // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AKitchenProjectCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AKitchenProjectCharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Use", IE_Pressed, this, &AKitchenProjectCharacter::UsePressed);
	InputComponent->BindAction("Use", IE_Released, this, &AKitchenProjectCharacter::UseReleased);

	InputComponent->BindAction("Select1", IE_Pressed, this, &AKitchenProjectCharacter::SelectSlot<1>);
	InputComponent->BindAction("Select2", IE_Pressed, this, &AKitchenProjectCharacter::SelectSlot<2>);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AKitchenProjectCharacter::FirePressed);
	InputComponent->BindAction("Fire", IE_Released, this, &AKitchenProjectCharacter::FireReleased);

	InputComponent->BindAction("AltFire", IE_Pressed, this, &AKitchenProjectCharacter::AltFirePressed);
	InputComponent->BindAction("AltFire", IE_Released, this, &AKitchenProjectCharacter::AltFireReleased);

	InputComponent->BindAction("Reload", IE_Pressed, this, &AKitchenProjectCharacter::ReloadPressed);
	InputComponent->BindAction("Reload", IE_Released, this, &AKitchenProjectCharacter::ReloadReleased);

	InputComponent->BindAction("Drop", IE_Pressed, this, &AKitchenProjectCharacter::DropPressed);
	InputComponent->BindAction("Drop", IE_Released, this, &AKitchenProjectCharacter::DropReleased);

	InputComponent->BindAction("Special", IE_Pressed, this, &AKitchenProjectCharacter::SpecialPressed);
	InputComponent->BindAction("Special", IE_Released, this, &AKitchenProjectCharacter::SpecialReleased);

	InputComponent->BindAxis("MoveForward", this, &AKitchenProjectCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AKitchenProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &AKitchenProjectCharacter::MouseLeftRight);

	InputComponent->BindAxis("TurnRate", this, &AKitchenProjectCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUpRate", this, &AKitchenProjectCharacter::LookUpAtRate);
	InputComponent->BindAxis("LookUp", this, &AKitchenProjectCharacter::MouseUpDown);
}


void AKitchenProjectCharacter::MouseUpDown(float Rate)
{
	AddControllerPitchInput(Rate);
	LookUpDown = Rate;
}

void AKitchenProjectCharacter::MouseLeftRight(float Rate)
{
	AddControllerYawInput(Rate);
	LookLeftRight = Rate;
}

void AKitchenProjectCharacter::BeginPlay()
{
	Super::BeginPlay();
	WeaponInventoryComponent->Initialize(this);
	CurrentHealth = MaxHealth;
	OriginalArmsRelativeLocation = ArmsMesh->RelativeLocation;
}

FString AKitchenProjectCharacter::GetName()
{
	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(PlayerState);
	return s->GetDisplayName();
}

FString AKitchenProjectCharacter::GetLabel()
{
	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(PlayerState);
	FString label = "Shove " + s->GetDisplayName();
	return label;
}


void AKitchenProjectCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (IsLocallyControlled())
	{
		float finaly;
		// arm sway code
		if (LookLeftRight != 0.0f)
		{
			float x = -LookLeftRight * 2.0f;
			finaly = FMath::FInterpTo(ArmsMesh->RelativeLocation.Y, OriginalArmsRelativeLocation.Y + x, deltaTime, 5.0f);
		}
		else
		{
			finaly = FMath::FInterpTo(ArmsMesh->RelativeLocation.Y, OriginalArmsRelativeLocation.Y, deltaTime, 10.0f);
		}

		float finalz;
		if (LookUpDown != 0.0f)
		{
			float y = LookUpDown * 2.0f;
			finalz = FMath::FInterpTo(ArmsMesh->RelativeLocation.Z, OriginalArmsRelativeLocation.Z + y, deltaTime, 5.0f);
		}
		else
		{
			finalz = FMath::FInterpTo(ArmsMesh->RelativeLocation.Z, OriginalArmsRelativeLocation.Z, deltaTime, 10.0f);
		}

		ArmsMesh->SetRelativeLocation(FVector(OriginalArmsRelativeLocation.X, finaly, finalz));
	}


	// if charging a weapon throw, add to charge
	if (IsHolding)
	{
		HoldingTime += deltaTime;
	}

	if (bIsDead && IsLocallyControlled())
	{
		if (IsValid(KillerActor))
		{
			FVector DirectionVector = KillerActor->GetActorLocation() - GetActorLocation();
			FRotator Rotation = DirectionVector.Rotation();
			GetFPCamera()->SetWorldRotation(Rotation);
		}
	}

	if (HasAuthority())
	{
		//Replicate the control rotation to everyone on the server
		ReplicateControlRotation(GetControlRotation());
	}

	// Only run if you own the pawn -- saves CPU cycles for everyone
	if (IsLocallyControlled())
	{
		// Check for usable objects
		static FName Tag = FName(TEXT("UsableTrace"));
		FVector StartPos = FPCamera->GetComponentLocation();
		FVector Forward = GetControlRotation().Vector();
		Forward.Normalize();
		FVector EndPos = StartPos + Forward * TotalRayTraceDistance;
		FCollisionQueryParams TraceParams(Tag, true, this);
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = false;
		FHitResult Hit(ForceInit);
		GetWorld()->LineTraceSingleByObjectType(Hit, StartPos, EndPos,
			FCollisionObjectQueryParams::AllObjects, TraceParams);
		
		// TODO: Clean all this up because it is nasty code
		// TODO: Replace all this with a separate class of its own
		if (Hit.bBlockingHit)
		{
			IUsableInterface* u = Cast<IUsableInterface>(Hit.GetActor());

			if (Hit.GetActor())
			{
				if (u)
				{
					// If the character is dead then don't let them interact with it
					AKitchenProjectCharacter* r = Cast<AKitchenProjectCharacter>(Hit.GetActor());
					if (r && r->bIsDead)
					{
						CurrentUsableObject = NULL;
						LastLookedAtObject = NULL;
						SignalUsableObject("NULL");
						return;
					}
					else
					{
						// If this is a newly looked at character from a character, fade the health bar in
						if (LastLookedAtObject)
						{
							if (LastLookedAtObject != u)
							{
								u->HoveredOn();
								LastLookedAtObject->HoveredOff();
								LastLookedAtObject = u;
							}
						}
						else
						{
							LastLookedAtObject = u;
							LastLookedAtObject->HoveredOn();
						}
					}
				}
			}
			else if (!Hit.GetActor() || !u)
			{
				// If I just recently stopped looking at a character to nothing, start fading its health bar out
				if (LastLookedAtObject)
				{
					LastLookedAtObject->HoveredOff();
					LastLookedAtObject = NULL;
				}
			}
		

			if (HasSelected)
			{
				if (u)
				{
					CurrentUsableObject = Hit.GetActor();
					if (Hit.GetActor() == Minion)
					{
						MoveToMarker->SetVisibility(false, true);
						SignalUsableObject(u->GetLabel());
					}
					else
					{
						MoveToMarker->SetVisibility(false, true);

						SignalUsableObject("Interact with " + u->GetName());
					}
				}
				else
				{
					CurrentUsableObject = NULL;
					HasMinionCommandLocation = true;

					MoveToMarker->SetWorldRotation(FRotator(0, 0, -90));
					MoveToMarker->SetVisibility(true, true);

					MinionCommandLocation = Hit.Location;
					MoveToMarker->SetWorldLocation(MinionCommandLocation);

					if (Minion)
					{
						FVector minionlocation = Minion->GetActorLocation();
						minionlocation.Z = 0;

						FVector hitlocation = MinionCommandLocation;
						hitlocation.Z = 0;

						FVector direction = minionlocation - hitlocation;
						FRotator rotation = direction.Rotation();
						rotation.Yaw -= 270;
						MoveToMarker->SetWorldRotation(rotation);
					}

					SignalUsableObject("Move There");
				}
				return;
			}

			MoveToMarker->SetVisibility(false, true);

			if (u != NULL)
			{
				CurrentUsableObject = Hit.GetActor();

				FVector v = Hit.Actor->GetActorLocation() - GetActorLocation();
				float distance = v.Size();

				if (Hit.Actor->IsA(AKitchenProjectCharacter::StaticClass()))
				{
					if (distance <= ShoveDistance)
					{
						// can shove player
						SignalUsableObject("Shove " + u->GetName());
						return;
					}
					else if (distance <= MinionRayTraceDistance && Hit.Actor->IsA(ATwoKitchenAICharacter::StaticClass()))
					{
						// yes you can display
						ATwoKitchenAICharacter* chara = Cast<ATwoKitchenAICharacter>(Hit.GetActor());
						if (chara->CanBossAround(this))
						{
							MoveToMarker->SetVisibility(false, true);

							SignalUsableObject(u->GetLabel());
						}
						else
						{
							SignalUsableObject("Select " + u->GetName());
						}
						return;
					}
				}
				else
				{
					if (distance <= ObjectRayTraceDistance)
					{
						SignalUsableObject(u->GetLabel());
						return;
					}
				}
				CurrentUsableObject = NULL;
			}
		}

		HasMinionCommandLocation = false;
		CurrentUsableObject = NULL;
		SignalUsableObject("NULL");
	}

}

// TODO: Put this in a separate class as a component of this class
void AKitchenProjectCharacter::ServerCommandMinion_Implementation(AActor* Object, FVector location, bool hasLocation)
{
	if (HasAuthority())
	{
		if (Object)
		{
			if (Object == Minion)
			{
				Minion->ServerSelectMinion(this);
				MoveToMarker->SetVisibility(false, true);

				MulticastPlaySound(CancelGenericAI, true);
			}
			else
			{
				IUsableInterface* u = Cast<IUsableInterface>(Object);
				if (u)
				{
					ATwoKitchenAIController* c = Cast<ATwoKitchenAIController>(Minion->GetController());
					c->UseObjectOrder(Object);

					MulticastPlaySound(OrderGenericAI, true);
				}
			}
			// Tell AI to use the Object
			//ATwoKitchenAICharacter* character = Cast<ATwoKitchenAICharacter>(M);
		}
		else if(hasLocation)
		{
			ATwoKitchenAIController* c = Cast<ATwoKitchenAIController>(Minion->GetController());
			c->MoveToOrder(location);

			MulticastPlaySound(OrderGenericAI, true);
		}
	}
}

// TODO: Put this in a separate class as a component of this class
bool AKitchenProjectCharacter::ServerCommandMinion_Validate(AActor* Object, FVector location, bool hasLocation)
{
	return true;
}

void AKitchenProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AKitchenProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

// pass local rotation on to the networked rotation, which is used for animation purposes seen by all clients
void AKitchenProjectCharacter::ReplicateControlRotation_Implementation(FRotator rot)
{
	NetworkedControlRotation = rot;
}

void AKitchenProjectCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AKitchenProjectCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AKitchenProjectCharacter::Debug()
{
}

void AKitchenProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AKitchenProjectCharacter, bIsDead);
	DOREPLIFETIME(AKitchenProjectCharacter, MaxHealth);
	DOREPLIFETIME(AKitchenProjectCharacter, CurrentHealth);
	DOREPLIFETIME(AKitchenProjectCharacter, Minion);
	DOREPLIFETIME(AKitchenProjectCharacter, HasSelected);
	DOREPLIFETIME(AKitchenProjectCharacter, MeshWeaponIndex);
}

void AKitchenProjectCharacter::NetworkedPlaySound_Implementation(USoundBase* sound, bool IsVoice)
{
	if (HasAuthority())
	{
		MulticastPlaySound(sound, IsVoice);
	}
}

bool AKitchenProjectCharacter::NetworkedPlaySound_Validate(USoundBase* sound, bool IsVoice)
{
	return true;
}

// Plays a sound coming from the player -- if it's the character's voice, it interrupts any currently playing voice file
// TODO: Put this in a separate class as a component of this class
void AKitchenProjectCharacter::MulticastPlaySound_Implementation(USoundBase* sound, bool IsVoice)
{
	if (IsVoice)
	{
		CharacterVoiceComponent->SetSound(sound);
		CharacterVoiceComponent->Play(0);
	}
	else
	{
		UGameplayStatics::SpawnSoundAttached(sound, GetRootComponent());
	}
}

// TODO: Put this in a separate class as a component of this class
void AKitchenProjectCharacter::UsePressed()
{
	IsHolding = true;

	GetWorldTimerManager().SetTimer(HoldTimer, this, &AKitchenProjectCharacter::UseHold, HoldSeconds);
}

void AKitchenProjectCharacter::UseReleased()
{
	IsHolding = false;

	if (HoldingTime < HoldSeconds)
	{
		Use();
		GetWorldTimerManager().ClearTimer(HoldTimer);
	}
	else
	{
		UseHold();
	}

	HoldingTime = 0;
}

// TODO: Put this in a separate class as a component of this class
void AKitchenProjectCharacter::UseHold()
{
	if (CurrentUsableObject == NULL)
		return;

	ServerHoldUseObject();
}

// TODO: Put this in a separate class as a component of this class
void AKitchenProjectCharacter::Use()
{
	if (CurrentUsableObject == NULL)
		return;

	if (HasSelected)
	{
		ServerCommandMinion(CurrentUsableObject, MinionCommandLocation, HasMinionCommandLocation);
	}
	else
	{
		ServerUseObject(CurrentUsableObject);
	}
}

// TODO: Put this in a separate class as a component of this class
void AKitchenProjectCharacter::ServerUseObject_Implementation(AActor* Object)
{
	if (HasAuthority())
	{
		IUsableInterface* u = Cast<IUsableInterface>(Object);
		if (u != NULL)
		{
			u->UseObject(this);
		}
	}
}

// TODO: Put this in a separate class as a component of this class
bool AKitchenProjectCharacter::ServerUseObject_Validate(AActor* Object)
{
	// do a check to see if you are close enough
	return true;
}

// TODO: Put this in a separate class as a component of this class
void AKitchenProjectCharacter::ServerHoldUseObject_Implementation()
{
	if (HasAuthority() && CurrentUsableObject != NULL)
	{
		IUsableInterface* u = Cast<IUsableInterface>(CurrentUsableObject);
		if (u != NULL)
		{
			u->HoldUseObject(this);
		}
	}
}

// TODO: Put this in a separate class as a component of this class
bool AKitchenProjectCharacter::ServerHoldUseObject_Validate()
{
	// do a check to see if you are close enough
	return true;
}

// TODO: Put this in a separate class as a component of this class
void AKitchenProjectCharacter::UseObject(AActor* Instigator)
{
	if (Instigator->IsA(AKitchenProjectCharacter::StaticClass()))
	{
		NetworkedPlaySound(ShoveSound, false);
		GetShoved(Instigator);
	}
}

void AKitchenProjectCharacter::GetShoved(AActor* Instigator)
{
	AKitchenProjectCharacter* c = Cast < AKitchenProjectCharacter>(Instigator);
	FVector v = c->GetActorLocation() - GetActorLocation();
	float distance = v.Size();
	if (distance <= c->ShoveDistance + 15.0f)
	{
		FVector forward = c->GetControlRotation().Vector();
		forward *= c->ShoveStrength;
		forward.Z = c->ShoveStrengthUp;
		GetCharacterMovement()->Launch(forward);
	}

	ForceOffComputer();
}

// Get selected by another player character
// TODO: Put this in a separate class as a component of this class
void AKitchenProjectCharacter::Selected(ATwoKitchenAICharacter* m)
{
	Minion = m;
	if (Minion)
		HasSelected = true;
	else
		HasSelected = false;
}

void AKitchenProjectCharacter::FirePressed()
{
	if(WeaponInventoryComponent->CurrentSelectedWeapon)
		WeaponInventoryComponent->CurrentSelectedWeapon->FirePressed();
}

void AKitchenProjectCharacter::FireReleased()
{
	if (WeaponInventoryComponent->CurrentSelectedWeapon)
		WeaponInventoryComponent->CurrentSelectedWeapon->FireReleased();
}

void AKitchenProjectCharacter::AltFirePressed()
{
	if (WeaponInventoryComponent->CurrentSelectedWeapon)
		WeaponInventoryComponent->CurrentSelectedWeapon->AltFirePressed();
}

void AKitchenProjectCharacter::AltFireReleased()
{
	if (WeaponInventoryComponent->CurrentSelectedWeapon)
		WeaponInventoryComponent->CurrentSelectedWeapon->AltFireReleased();
}

void AKitchenProjectCharacter::ReloadPressed()
{
	if (WeaponInventoryComponent->CurrentSelectedWeapon)
		WeaponInventoryComponent->CurrentSelectedWeapon->ReloadPressed();
}

void AKitchenProjectCharacter::ReloadReleased()
{
	if (WeaponInventoryComponent->CurrentSelectedWeapon)
		WeaponInventoryComponent->CurrentSelectedWeapon->ReloadReleased();
}

void AKitchenProjectCharacter::SpecialPressed()
{
	if (WeaponInventoryComponent->CurrentSelectedWeapon)
		WeaponInventoryComponent->CurrentSelectedWeapon->SpecialPressed();
}

void AKitchenProjectCharacter::SpecialReleased()
{
	if (WeaponInventoryComponent->CurrentSelectedWeapon)
		WeaponInventoryComponent->CurrentSelectedWeapon->SpecialReleased();
}

void AKitchenProjectCharacter::DropPressed()
{
	if (WeaponInventoryComponent->CurrentSelectedWeapon)
		WeaponInventoryComponent->CurrentSelectedWeapon->DropPressed();
}

void AKitchenProjectCharacter::DropReleased()
{
	if (WeaponInventoryComponent->CurrentSelectedWeapon)
		WeaponInventoryComponent->CurrentSelectedWeapon->DropReleased();
}

template<int32 Index>
void AKitchenProjectCharacter::SelectSlot()
{
	SelectSlot(Index);
}

void AKitchenProjectCharacter::SelectSlot(int32 slot)
{
	WeaponInventoryComponent->SelectSlot(slot);
}

void AKitchenProjectCharacter::ForceOffComputer()
{
	if (GetController()->IsA(ATwoKitchenPlayerController::StaticClass()))
	{
		ATwoKitchenPlayerController* contr = Cast<ATwoKitchenPlayerController>(GetController());
		if (contr->ComputerInUse != NULL)
		{
			contr->ComputerInUse->bIsInUse = false;
			contr->ComputerInUse->MulticastShowUsage(false);
			contr->ComputerInUse = NULL;
		}
	}
}

float AKitchenProjectCharacter::CustomTakeDamage(bool bDidCrit, FVector LaunchVector, FVector HitLocation, float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (CurrentHealth <= 0)
		return 0;

	ForceOffComputer();

	CurrentHealth -= Damage;

	if (CurrentHealth <= 0)
	{
		CurrentHealth = 0;
		Die(DamageCauser, LaunchVector, HitLocation);
	}
	MulticastDrawDamageUI(bDidCrit, EventInstigator, CurrentHealth, Damage, HitLocation);

	
	return Damage;
}

void AKitchenProjectCharacter::MulticastDieEffects_Implementation(AActor* Killer, FVector LaunchVector, FVector HitLocation)
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->AddImpulseAtLocation(LaunchVector, HitLocation, NAME_None);

	bIsDead = true;
	KillerActor = Killer;
	FPCamera->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = false;

	DieEffects();
}
void AKitchenProjectCharacter::Die(AActor* Killer, FVector LaunchVector, FVector HitLocation)
{
	DropReleased();

	ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(PlayerState);
	if (s)
	{
		if (s->GetKitchenManager())
		{
			s->GetKitchenManager()->AddMoney(s->GetDisplayName() + "'s hospital bills", -150);
		}

		AKitchenProjectCharacter* ca = Cast<AKitchenProjectCharacter>(Killer);
		if (ca)
		{
			ATwoKitchenPlayerState* b = Cast<ATwoKitchenPlayerState>(ca->PlayerState);
			if (s)
			{
				// check if you're on the same team
				if(b->GetKitchenManager() != s->GetKitchenManager())
					b->GetKitchenManager()->AddMoney("killed " + s->GetDisplayName(), 50);
			}
		}
	}


	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);

	MulticastDieEffects(Killer, LaunchVector, HitLocation);


	float time = 5.0f;
	ATwoKitchenGamestate* GameState = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());
	if (GameState)
	{
		time = GameState->RespawnTime;
	}

	// If this doesn't' have a player state it's an AI so don't do the rest
	if (PlayerState == NULL)
		return;

	ATwoKitchenPlayerState* p = Cast<ATwoKitchenPlayerState>(PlayerState);

	FTimerHandle DestroyTimer;
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AKitchenProjectCharacter::DestroySelf, time);

	// If it's an AI character who killed you
	if (Killer->IsA(ATwoKitchenAICharacter::StaticClass()))
	{
		// kill feed using ai character's name
		ATwoKitchenAICharacter* c = Cast<ATwoKitchenAICharacter>(Killer);
			
		ATwoKitchenGamemode* g = Cast<ATwoKitchenGamemode>(GetWorld()->GetAuthGameMode());
		g->SendConsoleMessage(p->GetDisplayName() + " was killed by " + c->DisplayName + "!");
	}
	// If it's a player character
	else if (Killer->IsA(AKitchenProjectCharacter::StaticClass()))
	{
		AKitchenProjectCharacter* c = Cast<AKitchenProjectCharacter>(Killer);

		ATwoKitchenGamemode* g = Cast<ATwoKitchenGamemode>(GetWorld()->GetAuthGameMode());
		ATwoKitchenPlayerState* q = Cast<ATwoKitchenPlayerState>(c->PlayerState);
		g->SendConsoleMessage(p->GetDisplayName() + " was killed by " + q->DisplayName + "!");
	}



	if (GetController())
	{
		// Add a death to the victim
		ATwoKitchenPlayerController* p = Cast<ATwoKitchenPlayerController>(GetController());
		if (p)
		{
			ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(p->PlayerState);
			if (s)
			{
				s->SetDeaths(s->Deaths + 1);
			}
		}
	}

	if (Killer)
	{
		// Give the killer a kill
		AKitchenProjectCharacter* c = Cast<AKitchenProjectCharacter>(Killer);
		if (c)
		{
			ATwoKitchenPlayerState* s = Cast<ATwoKitchenPlayerState>(c->PlayerState);
			if (s)
			{
				s->SetKills(s->Kills + 1);
			}
		}
	}
}

void AKitchenProjectCharacter::MulticastDrawDamageUI_Implementation(bool bIsCrit, AController* c, float UpdatedHealth, float Damage, FVector HitLocation)
{
	DrawDamageUI(bIsCrit, c, UpdatedHealth, Damage, HitLocation);
}

void AKitchenProjectCharacter::DestroySelf()
{
	// Lazy check if it is a player that died since I destroy the AI controllers instantly
	if (PlayerState)
	{
		if (PlayerState->IsA(ATwoKitchenPlayerState::StaticClass()))
		{
			if (HasAuthority())
			{
				ATwoKitchenGamemode* g = Cast<ATwoKitchenGamemode>(GetWorld()->GetAuthGameMode());
				ATwoKitchenPlayerController* p = Cast<ATwoKitchenPlayerController>(GetController());
				if (p && g)
				{
					g->RespawnPlayer(p);
				}

			}
		}
	}

	Destroy();
}


void AKitchenProjectCharacter::HoveredOn()
{
	ShowHealthBar();
}

void AKitchenProjectCharacter::HoveredOff()
{
	HideHealthBar();
}
