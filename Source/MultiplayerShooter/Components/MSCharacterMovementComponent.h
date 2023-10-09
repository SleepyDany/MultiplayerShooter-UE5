// Test Multiplayer Shooter Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MSCharacterMovementComponent.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API UMSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float WalkModifier = 0.5f;

	virtual float GetMaxSpeed() const override;
};
