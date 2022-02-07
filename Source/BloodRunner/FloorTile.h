// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HealthPotionItem.h"
#include "Obstacle.h"
#include "PointItem.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "FloorTile.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class BLOODRUNNER_API AFloorTile : public AActor
{
	GENERATED_BODY()

private:
	int32 ObsCount;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UArrowComponent* AttachPointArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UArrowComponent* CenterLaneArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UArrowComponent* LeftLaneArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UArrowComponent* RightLaneArrow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
	TSubclassOf<APointItem> SoulPointClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
	TSubclassOf<AObstacle> CoffinObstacleClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
	TSubclassOf<AObstacle> CartObstacleClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
	TSubclassOf<AObstacle> PostObstacleClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
	TSubclassOf<AHealthPotionItem> HealthPotionItemClass;

	// Sets default values for this actor's properties
	AFloorTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleInstanceOnly)
	class ARunnerGameModeBase* RunnerGameMode;

	UPROPERTY()
	FTimerHandle DestroyTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* FloorMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UBoxComponent* FloorTriggerBox;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="Components")
	TArray<AActor*> ChildActors;

	UPROPERTY(EditAnywhere, Category="Config")
	float SoulSpawnPercent[2] = {0.32f, 0.59f};
	UPROPERTY(EditAnywhere, Category="Config")
	float ObstacleSpawnPercent[2] = {0.6f, 0.9f};
	UPROPERTY(EditAnywhere, Category="Config")
	float PotionSpawnPercent[2] = {0.98f, 1.f};
	
	UPROPERTY(EditAnywhere, Category="Config")
	float FirstObsSpawnPercent[2] = {0.f, 0.45f};
	UPROPERTY(EditAnywhere, Category="Config")
	float SecondObsSpawnPercent[2] = {0.46f, 0.8f};
	UPROPERTY(EditAnywhere, Category="Config")
	float ThirdObsSpawnPercent[2] = {0.81f, 1.f};
	UPROPERTY(EditAnywhere, Category="Config")
	float SoulItemSpawnOffset = 400;


	UFUNCTION()
	void SpawnLaneItem(UArrowComponent* Lane);

public:
	UFUNCTION()
	void OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp,
	                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void DestroyFloorTile();

	UFUNCTION(BlueprintCallable)
	void SpawnItems();

	FORCEINLINE const FTransform& GetAttachTransform() const
	{
		return AttachPointArrow->GetComponentTransform();
	}
};
