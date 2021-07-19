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
	AFloorTile* Tile = AddFloorTile();

	if (Tile)
	{
		LaneSwitchingHorizontalValues.Add(Tile->LeftLaneArrow->GetComponentLocation().Y);
		LaneSwitchingHorizontalValues.Add(Tile->CenterLaneArrow->GetComponentLocation().Y);
		LaneSwitchingHorizontalValues.Add(Tile->RightLaneArrow->GetComponentLocation().Y);
	}

	for (float Val : LaneSwitchingHorizontalValues)
	{
		UE_LOG(LogTemp, Warning, TEXT("LaneValue: %f"), Val);
	}

	for (int i = 0; i < InitialFloorTilesCount; ++i)
	{
		AddFloorTile();
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
			NextFloorSpawnPoint = TileToSpawn->GetAttachTransform();
		}

		return TileToSpawn;
	}

	return nullptr;
}
