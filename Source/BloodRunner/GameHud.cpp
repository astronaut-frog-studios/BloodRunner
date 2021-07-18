// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHud.h"

#include "Components/TextBlock.h"

void UGameHud::InitializeGameHudPlayer(ARunCharacter* RunCharacter)
{
	if(RunCharacter)
	{
		PotionsCount->SetText(FText::AsNumber(RunCharacter->GetHealthPotions()));
		HealthBar->SetPercent(RunCharacter->GetPlayerHealth());

		RunCharacter->OnPotionsCountChange.AddDynamic(this, &UGameHud::SetPotionsCount);
		RunCharacter->OnHealthBarChange.AddDynamic(this, &UGameHud::SetHealth);
	}
}

void UGameHud::SetPotionsCount(int32 Count)
{
	PotionsCount->SetText(FText::AsNumber(Count));
}

void UGameHud::SetHealth(float Count)
{
	HealthBar->SetPercent(Count);
}

