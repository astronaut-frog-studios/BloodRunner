#include "RunCharacter.h"

// Sets default values
ARunCharacter::ARunCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlayerHealth = 0.3f;
}

// Called when the game starts or when spawned
void ARunCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARunCharacter::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARunCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARunCharacter::SetPlayerHealth(float const Health)
{
	PlayerHealth += Health;
	
	if(PlayerHealth <= 0)
	{
		PlayerHealth = 0;
	}
}

float ARunCharacter::GetPlayerHealth() const
{
	return PlayerHealth;
}
