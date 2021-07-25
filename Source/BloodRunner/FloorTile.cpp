// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorTile.h"

#include "RunCharacter.h"
#include "RunnerGameModeBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AFloorTile::AFloorTile()
{
	ObsCount = 0;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
	FloorMesh->SetupAttachment(SceneComponent);

	AttachPointArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("AttachPoint"));
	AttachPointArrow->SetupAttachment(SceneComponent);

	CenterLaneArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("CenterLane"));
	CenterLaneArrow->SetupAttachment(SceneComponent);

	RightLaneArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightLane"));
	RightLaneArrow->SetupAttachment(SceneComponent);

	LeftLaneArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftLane"));
	LeftLaneArrow->SetupAttachment(SceneComponent);

	FloorTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FloorTriggerBox"));
	FloorTriggerBox->SetupAttachment(SceneComponent);
	FloorTriggerBox->SetBoxExtent(FVector(32.0f, 500.0f, 200.0f));
	FloorTriggerBox->SetCollisionProfileName(TEXT("OverlapAll"));
}

// Called when the game starts or when spawned
void AFloorTile::BeginPlay()
{
	Super::BeginPlay();

	RunnerGameMode = Cast<ARunnerGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	check(RunnerGameMode);

	FloorTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorTile::OnTriggerBoxOverlap);
}

void AFloorTile::OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);

	if (RunCharacter)
	{
		RunnerGameMode->AddFloorTile();

		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AFloorTile::DestroyFloorTile, 4.0f, false);
	}
}

void AFloorTile::SpawnItems()
{
	if (IsValid(CoffinObstacleClass) && IsValid(CartObstacleClass) && IsValid(PostObstacleClass) && IsValid(HealthPotionItemClass))
	{
		SpawnLaneItem(CenterLaneArrow);
		SpawnLaneItem(LeftLaneArrow);
		SpawnLaneItem(RightLaneArrow);
	}
}

void AFloorTile::SpawnLaneItem(UArrowComponent* Lane)
{
	const float RandomPercentage = FMath::RandRange(0.f, 1.f);

	const FTransform& SpawnTransform = Lane->GetComponentTransform();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (UKismetMathLibrary::InRange_FloatFloat(RandomPercentage, SoulSpawnPercent[0], SoulSpawnPercent[1], true,
	                                           false) || ObsCount >= 2) //Alma
	{
		// TODO: spawnar a alma pra coletar, os pontos
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(RandomPercentage, ObstacleSpawnPercent[0], ObstacleSpawnPercent[1], true, false) && ObsCount < 2) //Obs
	{
		ObsCount++;

		const float ObstacleRandomPercentage = FMath::RandRange(0.f, 1.f);

		AObstacle* ObstacleToSpawn;

		if (UKismetMathLibrary::InRange_FloatFloat(ObstacleRandomPercentage, FirstObsSpawnPercent[0], FirstObsSpawnPercent[1], true, true)) // Caixao
		{
			ObstacleToSpawn = GetWorld()->SpawnActor<AObstacle>(CoffinObstacleClass, SpawnTransform,
			                                                    SpawnParameters);
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(ObstacleRandomPercentage, SecondObsSpawnPercent[0], SecondObsSpawnPercent[1], true, true)) // Obs2
		{
			ObstacleToSpawn = GetWorld()->SpawnActor<AObstacle>(CartObstacleClass, SpawnTransform,
			                                                    SpawnParameters);
		}
		else if (UKismetMathLibrary::InRange_FloatFloat(ObstacleRandomPercentage, ThirdObsSpawnPercent[0], ThirdObsSpawnPercent[1], true, true)) // Obs3
		{
			ObstacleToSpawn = GetWorld()->SpawnActor<AObstacle>(PostObstacleClass, SpawnTransform,
			                                                    SpawnParameters);
		}
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(RandomPercentage, PotionSpawnPercent[0], PotionSpawnPercent[1], true, true)) // Pot
	{
		AHealthPotionItem* PotionToSpawn = GetWorld()->SpawnActor<AHealthPotionItem>(
			HealthPotionItemClass, SpawnTransform,
			SpawnParameters);
	}
}

void AFloorTile::DestroyFloorTile()
{
	if (DestroyTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(DestroyTimerHandle);
	}

	Destroy();
}
