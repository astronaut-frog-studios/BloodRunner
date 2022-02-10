#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "RunCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPotionsCountChange, int32, PotionsCount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthBarHeal, float, HealthValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthBarMaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaUse, float, StaminaValue);

UCLASS()
class BLOODRUNNER_API ARunCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleInstanceOnly)
	class ARunnerGameModeBase* RunnerGameMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleInstanceOnly)
	bool bCanInitHud;
	UFUNCTION()
	void InitGameHud();

	UPROPERTY(VisibleInstanceOnly, Category="Movement")
	bool bIsPressingForwardAxis;

	UPROPERTY(VisibleInstanceOnly, Category="Health")
	bool bIsDead;

	UPROPERTY(VisibleInstanceOnly, Category="Stamina")
	bool bCanRegenStamina;
	UFUNCTION(Category="Stamina")
	void CheckPlayerStamina();

	UFUNCTION(Category="MyCamera")
	void SetupCamera();
	UFUNCTION(Category="MyCamera")
	void CameraFollowPlayer(float const DeltaTime);
	UPROPERTY(VisibleAnywhere, Category="MyCamera")
	bool bCameraCanFollow;
	UPROPERTY(EditAnywhere, Category="MyCamera")
	FVector InitialCameraOffset;
	UPROPERTY(EditAnywhere, Category="MyCamera")
	float NotFollowingInitialSeconds;
	UPROPERTY(EditAnywhere, Category="MyCamera")
	float NotFollowingMaxSeconds;

	UPROPERTY()
	FTimerHandle RestartTimeHandler;

	UPROPERTY()
	FTimerHandle DamageTimeHandler;

	UPROPERTY()
	FTimerHandle StaminaRegenTimeHandler;

public:
	// Sets default values for this character's properties
	ARunCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Assets")
	class UParticleSystem* DeathParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Assets")
	class USoundBase* DeathSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Assets")
	class USoundBase* HitSound;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="LaneSwitch")
	int32 CurrentLane;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="LaneSwitch")
	int32 NextLane;

	UPROPERTY(EditAnywhere, Category="Movement")
	float MaxSpeed;
	UPROPERTY(EditAnywhere, Category="Movement")
	float InitialSpeed;
	UPROPERTY(EditAnywhere, Category="Movement")
	float SprintSpeed;
	UPROPERTY(VisibleAnywhere, Category="Movement")
	float WalkSpeed;
	UPROPERTY(VisibleAnywhere, Category="Movement")
	float CameraSpeedX;
	UPROPERTY(EditAnywhere, Category="Movement")
	float MoveBackSpeed;

	UPROPERTY(VisibleAnywhere, Category="Health")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, Category="Health")
	float PlayerHealth;

	UPROPERTY(VisibleAnywhere, Category="Stamina")
	float MaxStamina;
	UPROPERTY(VisibleAnywhere, Category="Stamina")
	float PlayerStamina;
	UPROPERTY(VisibleAnywhere, Category="Stamina")
	float PlayerStaminaRegen;
	UPROPERTY(VisibleAnywhere, Category="Stamina")
	float PlayerStaminaConsume;

	UPROPERTY(EditAnywhere, Category="Potions")
	int32 MaxHealthPotions;
	UPROPERTY(EditAnywhere, Category="Potions")
	int32 HealthPotions;
	UPROPERTY(EditAnywhere, Category="Potions")
	float AmountToHeal;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Potions")
	class USoundBase* HealingSound;

	UPROPERTY(EditAnywhere)
	FName LevelToLoad;

private:
	UFUNCTION(Category="Movement")
	void MoveRight();
	UFUNCTION(Category="Movement")
	void MoveLeft();
	UFUNCTION(Category="Movement")
	void MoveNormal();
	UFUNCTION(Category="Movement")
	void MoveFaster();
	UFUNCTION(Category="Movement")
	void MoveBack();

	UFUNCTION(Category="Health")
	void OnDeath();

	UFUNCTION()
	void PushBackOnDamage();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent, Category="LaneSwitch")
	void ChangeLane();
	UFUNCTION(BlueprintCallable, Category="LaneSwitch")
	void ChangeLaneUpdate(float InterpolationValue);
	UFUNCTION(BlueprintCallable, Category="LaneSwitch")
	void ChangeLaneFinished();

	void IncrementSpeeds();

	UFUNCTION(BlueprintCallable, Category="MyCamera")
	float GetNotFollowingMaxSeconds() const;
	UFUNCTION(BlueprintCallable, Category="MyCamera")
	void SetNotFollowingMaxSeconds(float const Value);

	UFUNCTION(BlueprintImplementableEvent, Category="CameraAnim")
	void AnimCamera();
	UFUNCTION(BlueprintCallable, Category="CameraAnim")
	void AnimCameraUpdate(float InterpolationValue);
	UFUNCTION(BlueprintCallable, Category="CameraAnim")
	void AnimCameraFinished();

	UFUNCTION(BlueprintImplementableEvent, Category="DamageCamera")
	void AnimDamageCamera();
	UFUNCTION(BlueprintCallable, Category="DamageCamera")
	void DamageCameraUpdate(float const InterpolationValue) const;
	UFUNCTION(BlueprintCallable, Category="DamageCamera")
	void DamageCameraFinished() const;

	UFUNCTION(BlueprintCallable)
	void Attack();
	UFUNCTION(BlueprintCallable)
	void Shoot();

	UFUNCTION(BlueprintCallable, Category="Health")
	float GetPlayerHealth() const;
	UFUNCTION(BlueprintCallable, Category="Health")
	float GetPlayerRelativeHealth() const;
	UFUNCTION(BlueprintCallable, Category="Health")
	float GetPlayerMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category="Health")
	void IncrementPlayerHealth(float Health = 0.1f);
	UFUNCTION(BlueprintCallable, Category="Health")
	void SetPlayerHealth(float Health);
	UFUNCTION(BlueprintCallable, Category="Health")
	void UpgradeMaxHealth();
	UFUNCTION(BlueprintCallable, Category="Health")
	void OnHitReceived(float const Damage);
	UFUNCTION(BlueprintCallable, Category="Health")
	void Death();

	UFUNCTION(BlueprintCallable, Category="Potions")
	float GetHealthPotions() const;
	UFUNCTION(BlueprintCallable, Category="Potions")
	int32 GetMaxHealthPotions() const;
	UFUNCTION(BlueprintCallable, Category="Potions")
	void IncrementHealthPotions(int HealthPotion = 1);
	UFUNCTION(BlueprintCallable, Category="Potions")
	void SetHealthPotions(int Potions);
	UFUNCTION(BlueprintCallable, Category="Potions")
	void HealPlayer();

	UFUNCTION(BlueprintCallable, Category="Stamina")
	float GetPlayerStamina() const;
	UFUNCTION(BlueprintCallable, Category="Stamina")
	float GetPlayerMaxStamina() const;
	UFUNCTION(BlueprintCallable, Category="Stamina")
	float GetPlayerRelativeStamina() const;
	UFUNCTION(BlueprintCallable, Category="Stamina")
	void IncrementPlayerStamina(float Stamina = 0.1f);
	UFUNCTION(BlueprintCallable, Category="Stamina")
	void SetPlayerStamina(float Stamina);
	UFUNCTION(BlueprintCallable, Category="Stamina")
	void StartStaminaRegen();

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Delegates")
	FOnPotionsCountChange OnPotionsCountChange;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Delegates")
	FOnHealthBarHeal OnHealthBarHeal;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Delegates")
	FOnHealthBarMaxHealth OnHealthBarMaxHealth;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Delegates")
	FOnStaminaUse OnStaminaUse;
};
