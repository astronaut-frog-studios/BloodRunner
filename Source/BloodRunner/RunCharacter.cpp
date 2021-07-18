#include "RunCharacter.h"

#include "GameHud.h"
#include "RunnerGameModeBase.h"
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

	GameHud = Cast<ARunnerGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->GameHud;
	check(GameHud);
	GameHud->InitializeGameHudPlayer(this);
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

	OnHealthBarChange.Broadcast(PlayerHealth);

	if (PlayerHealth <= 0.01f)
	{
		PlayerHealth = 0.f;

		if (LevelToLoad != "")
		{
			UGameplayStatics::OpenLevel(this, LevelToLoad);
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

	OnPotionsCountChange.Broadcast(HealthPotions);
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

		if (HealingSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HealingSound, GetActorLocation());
		}
	}
	if (PlayerHealth >= MaxHealth)
	{
		SetPlayerHealth(MaxHealth);
	}

	OnHealthBarChange.Broadcast(PlayerHealth);
}
