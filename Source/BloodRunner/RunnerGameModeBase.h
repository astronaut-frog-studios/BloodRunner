// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "RunnerGameModeBase.generated.h"

class AFloorTile;

class UUserWidget;

UCLASS()
class BLOODRUNNER_API ARunnerGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category="Config")
	TSubclassOf<UUserWidget> GameHudClass;

	UPROPERTY(VisibleInstanceOnly, Category="Runtime")
	class UGameHud* GameHud;

	UPROPERTY(EditAnywhere, Category="Config")
	TSubclassOf<AFloorTile> FloorTileClass;

	UPROPERTY(EditAnywhere, Category="Config")
	int32 InitialFloorTilesCount = 10;
	
	UPROPERTY(EditAnywhere, Category="Runtime")
	FTransform NextFloorSpawnPoint;

	UPROPERTY(EditAnywhere, Category="Runtime")
	TArray<float> LaneSwitchingHorizontalValues;
	
	UFUNCTION(BlueprintCallable)
	void CreateInitialFloorTiles();

	UFUNCTION(BlueprintCallable)
	AFloorTile* AddFloorTile();
};
