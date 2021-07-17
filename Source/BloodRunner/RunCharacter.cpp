#include "RunCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARunCharacter::ARunCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	MaxHealth = 1.0f;
	PlayerHealth = MaxHealth;
	HealthPotions = 0;
	AmountToHeal = 0.2f;
	MaxHealthPotions = 6;
}

void ARunCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ARunCharacter::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARunCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Heal", IE_Pressed, this, &ARunCharacter::HealPlayer);
}

float ARunCharacter::GetPlayerHealth() const
{
	return PlayerHealth;
}

void ARunCharacter::SetPlayerHealth(float const Health)
{
	PlayerHealth = Health;
}

void ARunCharacter::IncrementPlayerHealth(float const Health)
{
	PlayerHealth += Health;

	if (PlayerHealth <= 0)
	{
		PlayerHealth = 0;

		if (LevelToLoad != "")
		{
			UGameplayStatics::OpenLevel(this,LevelToLoad, false);
		}
	}
}

float ARunCharacter::GetHealthPotions() const
{
	return HealthPotions;
}

void ARunCharacter::IncrementHealthPotions(int const HealthPotion)
{
	if (HealthPotions >= MaxHealthPotions)
	{
		HealthPotions = MaxHealthPotions;
		return;
	}

	HealthPotions += HealthPotion;
}

void ARunCharacter::SetHealthPotions(int const Potions)
{
	HealthPotions = Potions;
}

void ARunCharacter::HealPlayer()
{
	if (HealthPotions == 0) return;

	if (PlayerHealth < MaxHealth)
	{
		IncrementPlayerHealth(AmountToHeal);
		IncrementHealthPotions(-1);
	}
	if (PlayerHealth >= MaxHealth)
	{
		SetPlayerHealth(MaxHealth);
	}
}
