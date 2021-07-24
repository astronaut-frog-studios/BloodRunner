// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

UCLASS()
class BLOODRUNNER_API AObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	AObstacle();

protected:
	//virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UStaticMeshComponent* ObstacleMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UStaticMeshComponent* ObstacleOverlapBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	float DamageToPlayer;

	UFUNCTION(BlueprintCallable)
	void OnObstacleHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							UPrimitiveComponent* OtherComp,
							int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
