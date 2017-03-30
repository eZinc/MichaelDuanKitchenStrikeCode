// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "UnrealNetwork.h"
#include "UsableInterface.h"
#include "DamageableInterface.h"
#include "KitchenProjectCharacter.generated.h"

UCLASS(config = Game)
class AKitchenProjectCharacter : public ACharacter, public IDamageableInterface, public IUsableInterface
{
	GENERATED_BODY()

		/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FPCamera;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* MoveToMarker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ArmsMesh;

private:
	FRotator NetworkedControlRotation;

	UPROPERTY(EditAnywhere, Category = Sounds)
	UAudioComponent* CharacterVoiceComponent;

	UPROPERTY(EditDefaultsOnly, Category = Voices)
	USoundBase* SelectGenericAI;

	UPROPERTY(EditDefaultsOnly, Category = Voices)
	USoundBase* OrderGenericAI;

	UPROPERTY(EditDefaultsOnly, Category = Voices)
	USoundBase* CancelGenericAI;

	AActor* CurrentUsableObject;

	class IUsableInterface* LastLookedAtObject;

public:
	AKitchenProjectCharacter();

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Animation)
	int32 MeshWeaponIndex;

	USkeletalMeshComponent* GetArmsMesh()
	{
		return ArmsMesh;
	}

	void Use();

	virtual FString GetName() override;
	virtual FString GetLabel() override;
	virtual void UseObject(AActor* Instigator) override;

	virtual float CustomTakeDamage(bool bDidCrit, FVector LaunchVector, FVector HitLocation, float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser);

	FVector OriginalArmsRelativeLocation;

	virtual FVector GetAIUseLocation(AActor* Instigator) override
	{
		return GetActorLocation();
	}

	virtual float GetAIUseDistance(AActor* Instigator)
	{
		// shove only for now
		AKitchenProjectCharacter* u = Cast<AKitchenProjectCharacter>(Instigator);
		if (u)
		{
			return u->GetShoveDistance();
		}
		return 0;
	}

	float GetShoveDistance()
	{
		return ShoveDistance;
	}
	UPROPERTY(Replicated)
	class ATwoKitchenAICharacter* Minion;

	void Selected(class ATwoKitchenAICharacter* m);

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Gameplay)
	bool HasSelected = false;

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	USoundBase* ShoveSound;

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	USoundBase* ShoveVoice;

	UFUNCTION(BlueprintCallable, Category = KitchenProjectCharacter)
	FRotator GetNetworkedControlRotation()
	{
		return NetworkedControlRotation;
	}


	bool HasMinionCommandLocation = false;

	FVector MinionCommandLocation;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = KitchenProjectCharacter)
	void NetworkedPlaySound(USoundBase* sound, bool IsVoice);

	UFUNCTION(NetMulticast, Reliable, Category = KitchenProjectCharacter)
	void MulticastPlaySound(USoundBase* sound, bool IsVoice);

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicateControlRotation(FRotator rot);

	UFUNCTION(BlueprintImplementableEvent, Category = HUD)
	void SignalUsableObject(const FString& name);

	UFUNCTION(BlueprintImplementableEvent, Category = HUD)
	void DrawGameHUD();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseObject(AActor* Object);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCommandMinion(AActor* Object, FVector location, bool hasLocation);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, Category = Raytrace)
	float ObjectRayTraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = Raytrace)
	float MinionRayTraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = Raytrace)
	float TotalRayTraceDistance;

	void MouseUpDown(float Rate);
	void MouseLeftRight(float Rate);

	void UseHold();

	float LookUpDown;
	float LookLeftRight;

	void ForceOffComputer();

	FTimerHandle HoldTimer;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerHoldUseObject();

	UPROPERTY(EditDefaultsOnly, Category = Raytrace)
	float ShoveDistance;

	UPROPERTY(EditDefaultsOnly, Category = Shove)
	float ShoveStrength;

	UPROPERTY(EditDefaultsOnly, Category = Shove)
		float ShoveStrengthUp;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(Replicated)
	int32 fuckme = 0;

	void BeginPlay() override;

	void Tick(float deltaTime) override;


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	void GetShoved(AActor* Instigator);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHealthBar();

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

protected:

	AActor* KillerActor;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	void Debug();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFPCamera() const { return FPCamera; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UWeaponInventoryComponent* WeaponInventoryComponent;


	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastDrawDamageUI(bool bIsCrit, AController* c, float UpdatedHealth, float Damage, FVector HitLocation);

	UFUNCTION(BlueprintImplementableEvent)
	void DrawDamageUI(bool bIsCrit, AController* c, float UpdatedHealth, float Damage, FVector HitLocation);

	virtual void Die(AActor* Killer, FVector LaunchVector, FVector HitLocation);
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsDead = false;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDieEffects(AActor* Killer, FVector LaunchVector, FVector HitLocation);

	UFUNCTION(BlueprintImplementableEvent)
	void DieEffects();

	void DestroySelf();

	float HoldingTime = 0.0f;

	bool IsHolding = false;

	UPROPERTY(EditDefaultsOnly)
	float HoldSeconds = 0.6f;


	virtual void HoveredOn() override;
	virtual void HoveredOff() override;


private:
	virtual void FirePressed();
	virtual void FireReleased();
	virtual void AltFirePressed();
	virtual void AltFireReleased();
	virtual void ReloadPressed();
	virtual void ReloadReleased();
	virtual void SpecialPressed();
	virtual void SpecialReleased();
	virtual void DropPressed();
	virtual void DropReleased();
	virtual void UsePressed();
	virtual void UseReleased();

	template<int32 Index>
	void SelectSlot();

	virtual void SelectSlot(int32 slot);
};

