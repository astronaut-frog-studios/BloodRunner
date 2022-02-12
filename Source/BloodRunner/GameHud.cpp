// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHud.h"

#include "Components/TextBlock.h"

void UGameHud::InitializeGameHudPlayer(ARunCharacter* RunCharacter)
{
	if (RunCharacter)
	{
		PotionsCount->SetText(FText::AsNumber(RunCharacter->GetHealthPotions()));
		HealthBar->SetPercent(RunCharacter->GetPlayerHealth());

		RunCharacter->OnPotionsCountChange.AddDynamic(this, &UGameHud::SetPotionsCount);
		RunCharacter->OnHealthBarHeal.AddDynamic(this, &UGameHud::SetHealth);
		RunCharacter->OnHealthBarMaxHealth.AddDynamic(this, &UGameHud::UpgradeMaxHealth);
		RunCharacter->OnStaminaUse.AddDynamic(this, &UGameHud::SetStamina);
		RunCharacter->OnStaminaBarMaxStamina.AddDynamic(this,&UGameHud::UpgradeMaxStamina);
	}
}

void UGameHud::SetPotionsCount(int32 const Count)
{
	PotionsCount->SetText(FText::AsNumber(Count));
}

void UGameHud::SetHealth(float const Count)
{
	HealthBar->SetPercent(Count);
}

void UGameHud::UpgradeMaxHealth()
{
	FVector2D const HealthBarTranslation = HealthBar->RenderTransform.Translation;
	FVector2D const HealthBarScale = HealthBar->RenderTransform.Scale;

	HealthBar->SetRenderScale(FVector2D(HealthBarScale.X + 0.2f, HealthBarScale.Y));
	HealthBar->SetRenderTranslation(FVector2D(HealthBarTranslation.X + 20.f, HealthBarTranslation.Y));
}

void UGameHud::SetStamina(float const Value)
{
	StaminaBar->SetPercent(Value);
}

void UGameHud::UpgradeMaxStamina()
{
	FVector2D const StaminaBarTranslation = StaminaBar->RenderTransform.Translation;
	FVector2D const StaminaBarScale = StaminaBar->RenderTransform.Scale;

	StaminaBar->SetRenderScale(FVector2D(StaminaBarScale.X + 0.15f, StaminaBarScale.Y));
	StaminaBar->SetRenderTranslation(FVector2D(StaminaBarTranslation.X + 15.f, StaminaBarTranslation.Y));
}
