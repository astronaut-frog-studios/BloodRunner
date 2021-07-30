#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HunterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLOODRUNNER_API UHunterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Movement")
	float Speed;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Movement")
    class APawn* Pawn;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Movement")
    class UPawnMovementComponent* MovementComponent;
	

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

};
