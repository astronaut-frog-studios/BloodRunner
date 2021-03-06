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

	bCameraIsStationary = true;
	bIsDead = false;
	bCanRegenStamina = false;
	NotFollowingInitialSeconds = 2.f;
	InitialCameraOffset = FVector(-650.0, 0.f, 730);
	NotFollowingMaxSeconds = NotFollowingInitialSeconds;

	MaxHealth = 1.f;
	MaxUpgradedHealth = 2.2f;
	MaxHealthIncrementAmount = 0.3f;
	PlayerHealth = MaxHealth;

	HealthPotions = 0;
	AmountToHeal = 0.2f;
	MaxHealthPotions = 5;

	MaxStamina = 1.6f;
	PlayerStamina = MaxStamina;
	MaxUpgradedStamina = 3.2f;
	MaxStaminaIncrementAmount = 0.2f;
	PlayerStaminaConsume = 0.003f;
	PlayerStaminaRegen = 0.002f;

	CurrentLane = 1;
	NextLane = 0;
	MaxSpeed = 3600;
	InitialSpeed = 1060;
	SprintSpeed = 1600;
	MaxSprintSpeed = 4200;
	SprintSpeedMultiply = 1.025;
	WalkSpeedMultiply = 1.02;
	WalkSpeed = InitialSpeed;
	MoveBackSpeed = 200;
	CameraSpeedX = 100;

	SetupCamera();
	CameraSprintVfxComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SprintEffect"));
	CameraSprintVfxComponent->SetupAttachment(GetRootComponent());
}

void ARunCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	CameraArmComponent->SetAbsolute(true);

	RunnerGameMode = Cast<ARunnerGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	check(RunnerGameMode);

	bCanInitHud = true;
}

void ARunCharacter::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);

	InitGameHud();

	CameraFollowPlayer(DeltaTime);

	CheckPlayerStamina();
}

void ARunCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Heal", IE_Pressed, this, &ARunCharacter::HealPlayer);
	PlayerInputComponent->BindAction("MoveToRight", IE_Pressed, this, &ARunCharacter::MoveRight);
	PlayerInputComponent->BindAction("MoveToLeft", IE_Pressed, this, &ARunCharacter::MoveLeft);
	PlayerInputComponent->BindAction("MoveUp", IE_Pressed, this, &ARunCharacter::MoveFaster);
	PlayerInputComponent->BindAction("MoveUp", IE_Released, this, &ARunCharacter::MoveNormal);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ARunCharacter::Shoot);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ARunCharacter::Attack);
}

void ARunCharacter::InitGameHud()
{
	if (bCanInitHud)
	{
		RunnerGameMode->GameHud->InitializeGameHudPlayer(this);
		bCanInitHud = false;
	}
}

#pragma region Death
void ARunCharacter::OnHitReceived(float const Damage)
{
	IncrementPlayerHealth(-Damage);
	static FVector Impulse = FVector(-5000.f, 0, 0);
	GetCharacterMovement()->AddImpulse(Impulse, true);

	if (bCameraIsStationary)
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Invencible"));
		CameraArmComponent->TargetOffset.X = InitialCameraOffset.X - 300;
		GetWorldTimerManager().SetTimer(DamageTimeHandler, this, &ARunCharacter::PushBackOnDamage, 1.3f, false);
		AnimDamageCamera();
	}
	else if (!bCameraIsStationary)
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Invencible"));
		CameraArmComponent->TargetOffset.X = InitialCameraOffset.X;
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
	if (DamageTimeHandler.IsValid())
	{
		GetWorldTimerManager().ClearTimer(DamageTimeHandler);
	}

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
}

void ARunCharacter::Death()
{
	if (bIsDead)
	{
		return;
	}

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

		SetActorEnableCollision(false);
		GetMesh()->SetVisibility(false);

		World->GetTimerManager().SetTimer(RestartTimeHandler, this, &ARunCharacter::OnDeath, 2.0f);
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
	CameraArmComponent->TargetArmLength = 550.f;
	CameraArmComponent->TargetOffset = InitialCameraOffset;
	CameraArmComponent->bUsePawnControlRotation = true;
	CameraArmComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->SetFieldOfView(55.f);
	FRotator const CameraRotator = CameraComponent->GetComponentRotation();
	CameraComponent->SetWorldRotation(FRotator(-20.f, CameraRotator.Yaw, CameraRotator.Roll));
	CameraComponent->SetupAttachment(CameraArmComponent, USpringArmComponent::SocketName);
}

void ARunCharacter::CameraFollowPlayer(float const DeltaTime)
{
	FRotator ControlRotator = GetControlRotation();
	ControlRotator.Roll = 0.0f;
	ControlRotator.Pitch = 0.0f;
	AddMovementInput(ControlRotator.Vector());
	FVector const CameraArmLocation = CameraArmComponent->GetComponentLocation();
	FVector const CameraFollowPlayer = FVector(GetActorLocation().X - CameraSpeedX, CameraArmLocation.Y,
	                                           CameraArmLocation.Z);
	CameraArmComponent->SetRelativeLocation(CameraFollowPlayer);

	// SetNotFollowingMaxSeconds(GetNotFollowingMaxSeconds() - DeltaTime);
	//
	// if (NotFollowingMaxSeconds <= 0)
	// {
	// 	SprintAnimCamera();
	// }
}

#pragma region SprintCamera
void ARunCharacter::SprintAnimCameraUpdate(float const InterpolationValue)
{
	if (bIsPressingForwardAxis)
	{
		FVector NewCameraArmOffset = CameraArmComponent->TargetOffset;
		NewCameraArmOffset.X = FMath::Lerp(NewCameraArmOffset.X, InitialCameraOffset.X + 150, InterpolationValue);
		CameraArmComponent->TargetOffset = NewCameraArmOffset;
		return;
	}

	FVector NewCameraArmOffset = CameraArmComponent->TargetOffset;
	NewCameraArmOffset.X = FMath::Lerp(NewCameraArmOffset.X, InitialCameraOffset.X, InterpolationValue);
	CameraArmComponent->TargetOffset = NewCameraArmOffset;
	bCameraIsStationary = true;

	// SetNotFollowingMaxSeconds(NotFollowingInitialSeconds);
}

float ARunCharacter::GetNotFollowingMaxSeconds() const
{
	return NotFollowingMaxSeconds;
}

void ARunCharacter::SetNotFollowingMaxSeconds(float const Value)
{
	NotFollowingMaxSeconds = Value;
}
#pragma endregion SprintCamera

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

void ARunCharacter::IncrementSpeeds()
{
	if (SprintSpeed < MaxSprintSpeed)
	{
		SprintSpeed *= SprintSpeedMultiply;
	}

	if (WalkSpeed < MaxSpeed)
	{
		WalkSpeed *= WalkSpeedMultiply;

		if (!bIsPressingForwardAxis)
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}
	}
}

void ARunCharacter::MoveNormal()
{
	bIsPressingForwardAxis = false;

	CameraSprintVfxComponent->Deactivate();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	SprintAnimCamera();
}

void ARunCharacter::MoveFaster()
{
	if (PlayerStamina <= 0.002)
	{
		return;
	}

	if (bCameraIsStationary)
	{
		SprintAnimCamera();
	}

	FRotator ControlRotator = GetControlRotation();
	ControlRotator.Roll = 0.0f;
	ControlRotator.Pitch = 0.0f;

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	AddMovementInput(ControlRotator.Vector(), 1.0);

	if (!bIsPressingForwardAxis)
	{
		bCameraIsStationary = false;
		CameraSprintVfxComponent->Activate();
	}

	bIsPressingForwardAxis = true;
}

bool ARunCharacter::GetPressingForwardAxis() const
{
	return bIsPressingForwardAxis;
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
	if (HealthPotions == 0 || PlayerHealth >= MaxHealth) return;

	IncrementPlayerHealth(AmountToHeal);
	IncrementHealthPotions(-1);

	if (HealingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HealingSound, GetActorLocation());
	}

	if (PlayerHealth >= MaxHealth)
	{
		SetPlayerHealth(MaxHealth);
	}
}

void ARunCharacter::UpgradeMaxHealth()
{
	MaxHealth += MaxHealthIncrementAmount;

	if (MaxHealth >= MaxUpgradedHealth)
	{
		MaxHealth = MaxUpgradedHealth;
		SetPlayerHealth(MaxHealth);
		return;
	}

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

int32 ARunCharacter::GetMaxHealthPotions() const
{
	return MaxHealthPotions;
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

#pragma region Stamina
void ARunCharacter::IncrementPlayerStamina(float const Stamina)
{
	PlayerStamina += Stamina;

	OnStaminaUse.Broadcast(GetPlayerRelativeStamina());

	if (PlayerStamina >= MaxStamina)
	{
		SetPlayerStamina(MaxStamina);
	}
}

void ARunCharacter::UpgradeMaxStamina()
{
	MaxStamina += MaxStaminaIncrementAmount;

	if (MaxStamina >= MaxUpgradedStamina)
	{
		MaxStamina = MaxUpgradedStamina;
		SetPlayerStamina(MaxStamina);
		return;
	}

	OnStaminaBarMaxStamina.Broadcast();
}

void ARunCharacter::CheckPlayerStamina()
{
	if (bIsPressingForwardAxis)
	{
		bCanRegenStamina = false;

		IncrementPlayerStamina(-PlayerStaminaConsume);

		if (PlayerStamina <= 0.002)
		{
			GetWorldTimerManager().SetTimer(StaminaRegenTimeHandler, this, &ARunCharacter::StartStaminaRegen, 5.f,
			                                false);
			MoveNormal();
		}
	}
	else if (!bIsPressingForwardAxis)
	{
		if (!GetWorldTimerManager().TimerExists(StaminaRegenTimeHandler))
		{
			GetWorldTimerManager().SetTimer(StaminaRegenTimeHandler, this, &ARunCharacter::StartStaminaRegen, 1.2f,
			                                false);
		}

		if (bCanRegenStamina)
		{
			IncrementPlayerStamina(PlayerStaminaRegen);
		}
	}
}

float ARunCharacter::GetPlayerRelativeStamina() const
{
	return PlayerStamina / MaxStamina;
}

float ARunCharacter::GetPlayerMaxStamina() const
{
	return MaxStamina;
}

float ARunCharacter::GetPlayerStamina() const
{
	return PlayerStamina;
}

void ARunCharacter::SetPlayerStamina(float const Stamina)
{
	PlayerStamina = Stamina;
}

void ARunCharacter::StartStaminaRegen()
{
	if (StaminaRegenTimeHandler.IsValid())
	{
		GetWorldTimerManager().ClearTimer(StaminaRegenTimeHandler);
	}

	bCanRegenStamina = true;
}
#pragma endregion Stamina
