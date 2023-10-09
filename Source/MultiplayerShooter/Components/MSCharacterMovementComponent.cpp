// Test Multiplayer Shooter Game. All Rights Reserved.


#include "MSCharacterMovementComponent.h"

#include "MSBaseCharacter.h"

float UMSCharacterMovementComponent::GetMaxSpeed() const
{
    const float MaxSpeed = Super::GetMaxSpeed();
    const TObjectPtr<AMSBaseCharacter> baseCharacter = GetOwner<AMSBaseCharacter>();
    
    return (baseCharacter && baseCharacter->IsWalking()) ? MaxSpeed * WalkModifier : MaxSpeed;
}
