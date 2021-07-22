// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "RunCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPotionsCountChange, int32, PotionsCount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthBarChange, float, HealthValue);

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

	UPROPERTY(VisibleAnywhere, Category="Movement")
	bool bIsPressingForwardAxis;

	UFUNCTION(Category="MyCamera")
	void SetupCamera();
	UPROPERTY(VisibleAnywhere, Category="MyCamera")
	bool bCameraCanFollow;
	UPROPERTY(VisibleAnywhere,Category="MyCamera")
	FVector InitialCameraOffset;
	UPROPERTY(EditAnywhere, Category="MyCamera")
	float NotFollowingInitialSeconds;
	UPROPERTY(EditAnywhere, Category="MyCamera")
	float NotFollowingMaxSeconds;

public:
	// Sets default values for this character's properties
	ARunCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class UGameHud* GameHud;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="LaneSwitch")
	int32 CurrentLane;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="LaneSwitch")
	int32 NextLane;

	UPROPERTY(EditAnywhere, Category="Movement")
	float SprintSpeed;
	UPROPERTY(EditAnywhere, Category="Movement")
	float WalkSpeed;
	UPROPERTY(EditAnywhere, Category="Movement")
	float MoveBackSpeed;

	UPROPERTY(VisibleAnywhere, Category="Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, Category="Health")
	float PlayerHealth;

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

	UFUNCTION(BlueprintCallable, Category="Movement")
	void MoveRight();
	UFUNCTION(BlueprintCallable, Category="Movement")
	void MoveLeft();
	UFUNCTION(BlueprintCallable, Category="Movement")
	void MoveNormal();
	UFUNCTION(BlueprintCallable, Category="Movement")
	void MoveFaster();
	UFUNCTION(BlueprintCallable, Category="Movement")
	void MoveBack();

	UFUNCTION(BlueprintCallable)
	void Attack();
	UFUNCTION(BlueprintCallable)
	void Shoot();

	UFUNCTION(BlueprintCallable)
	void HealPlayer();

	UFUNCTION(BlueprintCallable)
	float GetPlayerHealth() const;
	UFUNCTION(BlueprintCallable)
	void IncrementPlayerHealth(float Health = 0.1f);
	UFUNCTION(BlueprintCallable)
	void SetPlayerHealth(float Health);

	UFUNCTION(BlueprintCallable)
	float GetHealthPotions() const;
	UFUNCTION(BlueprintCallable)
	void IncrementHealthPotions(int HealthPotion = 1);
	UFUNCTION(BlueprintCallable)
	void SetHealthPotions(int Potions);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Delegates")
	FOnPotionsCountChange OnPotionsCountChange;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Delegates")
	FOnHealthBarChange OnHealthBarChange;
};
