// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerGameModeBase.h"

#include "FloorTile.h"
#include "GameHud.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ARunnerGameModeBase::BeginPlay()
{
	// UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

	GameHud = Cast<UGameHud>(CreateWidget(GetWorld(), GameHudClass));
	check(GameHud);

	GameHud->AddToViewport();

	CreateInitialFloorTiles();
}

void ARunnerGameModeBase::CreateInitialFloorTiles()
{
	for (int i = 0; i < InitialFloorTilesCount; ++i)
	{
		AFloorTile* Tile  = AddFloorTile();

		if(i == 0 && Tile)
		{
				LaneSwitchingHorizontalValues.Add(Tile->LeftLaneArrow->GetComponentLocation().Y);
				LaneSwitchingHorizontalValues.Add(Tile->CenterLaneArrow->GetComponentLocation().Y);
				LaneSwitchingHorizontalValues.Add(Tile->RightLaneArrow->GetComponentLocation().Y);
		}
	}
}

AFloorTile* ARunnerGameModeBase::AddFloorTile()
{
	UWorld* CurrentWorld = GetWorld();

	if (CurrentWorld)
	{
		AFloorTile* TileToSpawn = CurrentWorld->SpawnActor<AFloorTile>(FloorTileClass, NextFloorSpawnPoint);

		if (IsValid(TileToSpawn)) // == if(TitleToSpawn)
		{
			FTransform const TileTransform = TileToSpawn->AttachPointArrow->GetComponentTransform();

			NextFloorSpawnPoint = TileTransform;
		}

		return TileToSpawn;
	}

	return nullptr;
}
