#include "RunCharacter.h"

#include "GameHud.h"
#include "RunnerGameModeBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARunCharacter::ARunCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bCameraCanFollow = true;
	NotFollowingInitialSeconds = 2.8f;
	InitialCameraOffset = FVector(-500.0, 0.f, 400);
	NotFollowingMaxSeconds = NotFollowingInitialSeconds;

	MaxHealth = 1.f;
	PlayerHealth = MaxHealth;
	HealthPotions = 0;
	AmountToHeal = 0.2f;
	MaxHealthPotions = 6;
	CurrentLane = 1;
	NextLane = 0;
	SprintSpeed = 1200;
	WalkSpeed = 600;
	MoveBackSpeed = 200;

	SetupCamera();
}

void ARunCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraArmComponent->SetAbsolute(true);

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

	if (bCameraCanFollow)
	{
		FVector const CameraArmLocation = CameraArmComponent->GetComponentLocation();
		FVector const CameraFollowPlayer = FVector(GetActorLocation().X, CameraArmLocation.Y,
		                                           CameraArmLocation.Z);
		CameraArmComponent->SetWorldLocation(CameraFollowPlayer);
	}
	else if (!bCameraCanFollow)
	{
		SetNotFollowingMaxSeconds(GetNotFollowingMaxSeconds() - DeltaTime);

		if(NotFollowingMaxSeconds <= 0)
		{
			AnimCamera();
		}
	}
}

#pragma region Camera
void ARunCharacter::SetupCamera()
{
	CameraArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArmComponent->TargetArmLength = 450.f;
	CameraArmComponent->TargetOffset = InitialCameraOffset;
	CameraArmComponent->bUsePawnControlRotation = true;
	CameraArmComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->SetFieldOfView(45.f);
	FRotator const CameraRotator = CameraComponent->GetComponentRotation();
	CameraComponent->SetWorldRotation(FRotator(-12.5f, CameraRotator.Yaw, CameraRotator.Roll));
	CameraComponent->SetupAttachment(CameraArmComponent, USpringArmComponent::SocketName);
}

void ARunCharacter::AnimCameraUpdate(float const InterpolationValue)
 {
 	FVector CameraArmLocation = CameraArmComponent->GetComponentLocation();
 	CameraArmLocation.X = FMath::Lerp(CameraArmLocation.X, GetActorLocation().X, InterpolationValue);
 
 	CameraArmComponent->SetWorldLocation(CameraArmLocation);

	SetNotFollowingMaxSeconds(NotFollowingInitialSeconds);
 }

void ARunCharacter::AnimCameraFinished()
{
	bCameraCanFollow = true;
}

float ARunCharacter::GetNotFollowingMaxSeconds() const
{
	return NotFollowingMaxSeconds;
}

void ARunCharacter::SetNotFollowingMaxSeconds(float const Value)
{
	NotFollowingMaxSeconds = Value;
}
#pragma endregion Camera

#pragma region Movement
void ARunCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Heal", IE_Pressed, this, &ARunCharacter::HealPlayer);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARunCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARunCharacter::StopJumping);
	PlayerInputComponent->BindAction("MoveToRight", IE_Pressed, this, &ARunCharacter::MoveRight);
	PlayerInputComponent->BindAction("MoveToLeft", IE_Pressed, this, &ARunCharacter::MoveLeft);
	PlayerInputComponent->BindAction("MoveUp", IE_Pressed, this, &ARunCharacter::MoveFaster);
	PlayerInputComponent->BindAction("MoveUp", IE_Released, this, &ARunCharacter::MoveNormal);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ARunCharacter::Shoot);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ARunCharacter::Attack);
}

void ARunCharacter::ChangeLaneUpdate(float const InterpolationValue)
{
	FVector PlayerLocation = GetCapsuleComponent()->GetComponentLocation();

	PlayerLocation.Y = FMath::Lerp(RunnerGameMode->LaneSwitchingHorizontalValues[CurrentLane],
	                               RunnerGameMode->LaneSwitchingHorizontalValues[NextLane], InterpolationValue);

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

void ARunCharacter::MoveNormal()
{
	if (!bCameraCanFollow)
	{
		AnimCamera();
	}

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	bIsPressingForwardAxis = false;
}

void ARunCharacter::MoveFaster()
{
	FRotator ControlRotator = GetControlRotation();
	ControlRotator.Roll = 0.0f;
	ControlRotator.Pitch = 0.0f;
	// const FRotator Yaw(0.f, ControlRotator.Yaw, 0.f);
	// FVector direction = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	AddMovementInput(ControlRotator.Vector(), 1.0);

	if (!bIsPressingForwardAxis)
	{
		bCameraCanFollow = false;
	}

	bIsPressingForwardAxis = true;
}
#pragma endregion Movement

#pragma region Actions
void ARunCharacter::MoveBack()
{
	UE_LOG(LogTemp, Error, TEXT("Action not implemented....under development"));
}

void ARunCharacter::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
}

void ARunCharacter::Shoot()
{
	UE_LOG(LogTemp, Warning, TEXT("Shoot"));
}
#pragma endregion Actions

#pragma region PlayerHealth
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

float ARunCharacter::GetPlayerHealth() const
{
	return PlayerHealth;
}

void ARunCharacter::SetPlayerHealth(float const Health)
{
	PlayerHealth = Health;
}

#pragma endregion PlayerHealth

#pragma region HealthPotions
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
#pragma endregion HealthPotions
