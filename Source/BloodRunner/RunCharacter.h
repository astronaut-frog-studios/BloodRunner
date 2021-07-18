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

public:
	// Sets default values for this character's properties
	ARunCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class UGameHud* GameHud;


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

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Delegates")
	FOnPotionsCountChange OnPotionsCountChange;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Delegates")
	FOnHealthBarChange OnHealthBarChange;

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

	UFUNCTION(BlueprintCallable)
	void HealPlayer();
};
