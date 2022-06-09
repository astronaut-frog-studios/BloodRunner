// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerGameModeBase.h"

#include "FloorTile.h"
#include "GameHud.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


void ARunnerGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

	GameHud = Cast<UGameHud>(CreateWidget(GetWorld(), GameHudClass));
	check(GameHud);
	GameHud->AddToViewport();

	CreateInitialFloorTiles();
}

#pragma region FloorTiles
void ARunnerGameModeBase::CreateInitialFloorTiles()
{
	for (int i = 0; i < InitialFloorTilesCount; ++i)
	{
		const bool bCanSpawnItems = (i == 0 || i == 1 || i == 2) ? false : true;

		AFloorTile* Tile = AddFloorTile(bCanSpawnItems);

		if (i == 0 && Tile)
		{
			LaneSwitchingHorizontalValues.Add(Tile->LeftLaneArrow->GetComponentLocation().Y);
			LaneSwitchingHorizontalValues.Add(Tile->CenterLaneArrow->GetComponentLocation().Y);
			LaneSwitchingHorizontalValues.Add(Tile->RightLaneArrow->GetComponentLocation().Y);
		}
	}
}

AFloorTile* ARunnerGameModeBase::AddFloorTile(const bool bSpawnItems)
{
	UWorld* CurrentWorld = GetWorld();

	if (CurrentWorld)
	{
		AFloorTile* TileToSpawn = CurrentWorld->SpawnActor<AFloorTile>(FloorTileClass, NextFloorSpawnPoint);

		if (IsValid(TileToSpawn)) // == if(TitleToSpawn)
		{
			FloorTilesCount ++;
			bool bCanSpawnLantern = BIsFloorTileCountDividedByTen();

			if (bSpawnItems)
			{
				TileToSpawn->SpawnItems();
			}

			FTransform const TileTransform = TileToSpawn->AttachPointArrow->GetComponentTransform();

			NextFloorSpawnPoint = TileTransform;
		}

		return TileToSpawn;
	}

	return nullptr;
}

bool ARunnerGameModeBase::BIsFloorTileCountDividedByTen() const
{
	if (FloorTilesCount % 10 == 0)
	{
		return true;
	}

	return false;
}
#pragma endregion FloorTiles

#pragma region Souls
int32 ARunnerGameModeBase::GetSoulPoints() const
{
	return SoulPoints;
}

void ARunnerGameModeBase::IncrementSoulPoints(int32 const Value)
{
	SoulPoints += Value;
}

void ARunnerGameModeBase::SetSoulPoints(int32 const Value)
{
	SoulPoints = Value;
}
#pragma endregion Souls

#pragma region Distance
int32 ARunnerGameModeBase::GetDistancePoints() const
{
	return DistancePoints;
}

void ARunnerGameModeBase::IncrementDistancePoints(int32 const Value)
{
	DistancePoints += Value * DistancePointsMultiplier;

	OnDistancePointsChange.Broadcast(DistancePoints);
}

void ARunnerGameModeBase::SetDistancePointsMultiplier(int32 const Value)
{
	DistancePointsMultiplier = Value;
}
#pragma endregion Distance