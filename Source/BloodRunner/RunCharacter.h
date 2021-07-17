// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RunCharacter.generated.h"

UCLASS()
class BLOODRUNNER_API ARunCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARunCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	float MaxHealth;
	UPROPERTY(EditAnywhere)
	float PlayerHealth;

	UPROPERTY(EditAnywhere)
	int32 MaxHealthPotions;
	UPROPERTY(EditAnywhere)
	int32 HealthPotions;
	UPROPERTY(EditAnywhere)
	float AmountToHeal;

	UPROPERTY(EditAnywhere)
	FName LevelToLoad;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	float GetPlayerHealth() const;
	UFUNCTION(BlueprintCallable)
	void IncrementPlayerHealth(float Health = 0.1f);
	UFUNCTION(BlueprintCallable)
	void SetPlayerHealth(float Health) ;

	UFUNCTION(BlueprintCallable)
	float GetHealthPotions() const;
	UFUNCTION(BlueprintCallable)
	void IncrementHealthPotions(int HealthPotion = 1);
	UFUNCTION(BlueprintCallable)
	void SetHealthPotions(int Potions);

	UFUNCTION(BlueprintCallable)
	void HealPlayer();
};
