// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerGameModeBase.h"

#include "GameHud.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ARunnerGameModeBase::BeginPlay()
{
	// UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

	GameHud = Cast<UGameHud>(CreateWidget(GetWorld(), GameHudClass));
	check(GameHud);

	GameHud->AddToViewport();
}
