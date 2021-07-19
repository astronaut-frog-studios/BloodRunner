#include "RunCharacter.h"

#include "GameHud.h"
#include "RunnerGameModeBase.h"
#include "Components/CapsuleComponent.h"
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

	RunnerGameMode = Cast<ARunnerGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	check(RunnerGameMode);
	
	GameHud = RunnerGameMode->GameHud;
	check(GameHud);
	GameHud->InitializeGameHudPlayer(this);
}

void ARunCharacter::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator ControlRotator = GetControlRotation();
	ControlRotator.Roll = 0.0f;
	ControlRotator.Pitch = 0.0f;

	AddMovementInput(ControlRotator.Vector());
}

void ARunCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Heal", IE_Pressed, this, &ARunCharacter::HealPlayer);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARunCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARunCharacter::StopJumping);
	PlayerInputComponent->BindAction("MoveToRight", IE_Pressed, this, &ARunCharacter::MoveRight);
	PlayerInputComponent->BindAction("MoveToLeft", IE_Pressed, this, &ARunCharacter::MoveLeft);
	PlayerInputComponent->BindAction("MoveUp", IE_Pressed, this, &ARunCharacter::MoveUp);
	PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &ARunCharacter::MoveDown);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ARunCharacter::Shoot);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ARunCharacter::Attack);
}

void ARunCharacter::ChangeLaneUpdate(float const InterpolationValue)
{
	FVector PlayerLocation = GetCapsuleComponent()->GetComponentLocation();

	PlayerLocation.Y = FMath::Lerp(RunnerGameMode->LaneSwitchingHorizontalValues[CurrentLane],RunnerGameMode->LaneSwitchingHorizontalValues[NextLane], InterpolationValue );

	SetActorLocation(PlayerLocation);
}

void ARunCharacter::ChangeLaneFinished()
{
	CurrentLane = NextLane;
}

void ARunCharacter::MoveRight()
{
	NextLane = FMath::Clamp(CurrentLane + 1, 0, 2);
	ChangeLane();
}

void ARunCharacter::MoveLeft()
{
	NextLane = FMath::Clamp(CurrentLane - 1, 0, 2);
	ChangeLane();
}

void ARunCharacter::MoveUp()
{
	UE_LOG(LogTemp, Warning, TEXT("MoveForward"));
}

void ARunCharacter::MoveDown()
{
	UE_LOG(LogTemp, Warning, TEXT("MoveBackwards"));

}

void ARunCharacter::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack"));

}

void ARunCharacter::Shoot()
{
	UE_LOG(LogTemp, Warning, TEXT("Shoot"));

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
