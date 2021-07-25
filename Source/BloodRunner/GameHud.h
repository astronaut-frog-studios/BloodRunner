// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "RunCharacter.h"
#include "RunnerGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"

#include "GameHud.generated.h"

/**
 * 
 */
UCLASS()
class BLOODRUNNER_API UGameHud : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* PotionsCount;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* HealthBar;

public:
	UFUNCTION(BlueprintCallable)
	void InitializeGameHudPlayer(ARunCharacter* RunCharacter);

	UFUNCTION(BlueprintCallable)
	void SetPotionsCount(int32 const Count);

	UFUNCTION(BlueprintCallable)
	void SetHealth(float const Count);

	UFUNCTION(BlueprintCallable)
	void UpgradeMaxHealth();
};