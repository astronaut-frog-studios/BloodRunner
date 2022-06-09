// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "RunnerGameModeBase.generated.h"

class AFloorTile;

class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDistancePointsChange, int32, DistancePoints);

UCLASS()
class BLOODRUNNER_API ARunnerGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	int32 SoulPoints = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 DistancePoints = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 DistancePointsMultiplier = 1;

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
	UPROPERTY(EditAnywhere, Category="Runtime")
	int FloorTilesCount;
	UFUNCTION(BlueprintCallable, Category="Runtime")
	bool BIsFloorTileCountDividedByTen() const;

	UFUNCTION(BlueprintCallable)
	void CreateInitialFloorTiles();
	UFUNCTION(BlueprintCallable)
	AFloorTile* AddFloorTile(const bool bSpawnItems = true);

	UFUNCTION(BlueprintCallable)
	int32 GetSoulPoints() const;
	UFUNCTION(BlueprintCallable)
	void IncrementSoulPoints(int32 const Value = 1);
	UFUNCTION(BlueprintCallable)
	void SetSoulPoints(int32 const Value = 1);

	UFUNCTION(BlueprintCallable)
	int32 GetDistancePoints() const;
	UFUNCTION(BlueprintCallable)
	void IncrementDistancePoints(int32 const Value = 1);
	UFUNCTION(BlueprintCallable)
	void SetDistancePointsMultiplier(int32 const Value = 1);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Delegates")
	FOnDistancePointsChange OnDistancePointsChange;
};
