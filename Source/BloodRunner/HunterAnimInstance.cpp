#include "HunterAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"

void UHunterAnimInstance::NativeInitializeAnimation()
{
    if(Pawn == nullptr)
    {
        Pawn = TryGetPawnOwner();
    }

    if(IsValid(Pawn))
    {
        MovementComponent = Pawn->GetMovementComponent();
    }
}

void UHunterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    if(Pawn && MovementComponent)
    {
        Speed = Pawn->GetVelocity().Size();
    }
    
}
