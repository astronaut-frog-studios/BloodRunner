#include "Obstacle.h"

#include "RunCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AObstacle::AObstacle()
{
	DamageToPlayer = 0.2f;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;

	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	ObstacleMesh->SetupAttachment(SceneComponent);

	ObstacleOverlapBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleTriggerBox"));
	ObstacleOverlapBox->SetupAttachment(ObstacleMesh);
	ObstacleOverlapBox->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	ObstacleOverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AObstacle::OnObstacleHit);
}

void AObstacle::OnObstacleHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);

		if (IsValid(RunCharacter))
		{
			RunCharacter->OnHitReceived(DamageToPlayer);
		}
	}
}
