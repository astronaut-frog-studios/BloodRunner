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
	bIsDead = false;
	NotFollowingInitialSeconds = 2.f;
	InitialCameraOffset = FVector(-540.0, 0.f, 680);
	NotFollowingMaxSeconds = NotFollowingInitialSeconds;

	MaxHealth = 1.f;
	PlayerHealth = MaxHealth;
	HealthPotions = 0;
	AmountToHeal = 0.2f;
	MaxHealthPotions = 6;
	CurrentLane = 1;
	NextLane = 0;
	SprintSpeed = 1500;
	WalkSpeed = 800;
	MoveBackSpeed = 200;

	SetupCamera();
}

void ARunCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

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

		if (NotFollowingMaxSeconds <= 0)
		{
			AnimCamera();
		}
	}
}

#pragma region Death
void ARunCharacter::OnHitReceived(float const Damage)
{
	IncrementPlayerHealth(-Damage);

	static FVector Impulse = FVector(-5000.f, 0, 0);

	GetCharacterMovement()->AddImpulse(Impulse, true);
	if(bCameraCanFollow)
	{
		CameraArmComponent->TargetOffset.X = InitialCameraOffset.X - 300;
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Invencible"));
		GetWorldTimerManager().SetTimer(DamageTimeHandler, this, &ARunCharacter::PushBackOnDamage, 1.3f, false);
		AnimDamageCamera();
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
	}

	if (GetPlayerHealth() <= 0.f)
	{
		Death();
	}
}

void ARunCharacter::PushBackOnDamage()
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	
	if (DamageTimeHandler.IsValid())
	{
		GetWorldTimerManager().ClearTimer(DamageTimeHandler);
	}
}

void ARunCharacter::Death()
{
	if (!bIsDead)
	{
		const FVector PlayerLocation = GetActorLocation();

		UWorld* World = GetWorld();

		if (World)
		{
			bIsDead = true;
			DisableInput(nullptr);
			GetCharacterMovement()->MaxWalkSpeed = 0;

			if (DeathParticleSystem) //TODO: Death effect, Bloodborne deadh effect on screen
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, DeathParticleSystem, PlayerLocation);
			}

			if (DeathSound)
			{
				UGameplayStatics::PlaySoundAtLocation(World, DeathSound, PlayerLocation);
			}

			GetMesh()->SetVisibility(false);

			World->GetTimerManager().SetTimer(RestartTimeHandler, this, &ARunCharacter::OnDeath, 2.0f);
		}
	}
}

void ARunCharacter::OnDeath()
{
	bIsDead = false;

	if (RestartTimeHandler.IsValid())
	{
		GetWorldTimerManager().ClearTimer(RestartTimeHandler);
	}

	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("RestartLevel"));
}

#pragma endregion Death

#pragma region Camera
void ARunCharacter::SetupCamera()
{
	CameraArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArmComponent->TargetArmLength = 460.f;
	CameraArmComponent->TargetOffset = InitialCameraOffset;
	CameraArmComponent->bUsePawnControlRotation = true;
	CameraArmComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->SetFieldOfView(50.f);
	FRotator const CameraRotator = CameraComponent->GetComponentRotation();
	CameraComponent->SetWorldRotation(FRotator(-20.f, CameraRotator.Yaw, CameraRotator.Roll));
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

void ARunCharacter::DamageCameraUpdate(float const InterpolationValue) const
{
	FVector NewCameraArmOffset = CameraArmComponent->TargetOffset;
	NewCameraArmOffset.X = FMath::Lerp(NewCameraArmOffset.X, InitialCameraOffset.X, InterpolationValue);

	CameraArmComponent->TargetOffset = NewCameraArmOffset;
}

void ARunCharacter::DamageCameraFinished() const
{
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

	OnHealthBarHeal.Broadcast(GetPlayerRelativeHealth());
}

void ARunCharacter::UpgradeMaxHealth()
{
	if(MaxHealth >= 2.4f)
	{
		return;
	}
		MaxHealth += 0.3;

		OnHealthBarMaxHealth.Broadcast();
}

void ARunCharacter::IncrementPlayerHealth(float const Health)
{
	PlayerHealth += Health;

	OnHealthBarHeal.Broadcast(GetPlayerRelativeHealth());

	if (PlayerHealth <= 0.01f)
	{
		PlayerHealth = 0.f;

		if (LevelToLoad != "")
		{
			UGameplayStatics::OpenLevel(this, LevelToLoad);
		}
	}
}

float ARunCharacter::GetPlayerRelativeHealth() const
{
	return PlayerHealth / MaxHealth;
}

float ARunCharacter::GetPlayerMaxHealth() const
{
	return MaxHealth;
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
