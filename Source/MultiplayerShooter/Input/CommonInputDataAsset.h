// Test Multiplayer Shooter Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputDataAsset.h"
#include "CommonInputDataAsset.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API UCommonInputDataAsset : public UInputDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TObjectPtr<UInputAction> IA_Look;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TObjectPtr<UInputAction> IA_Move;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TObjectPtr<UInputAction> IA_Jump;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TObjectPtr<UInputAction> IA_Walk;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TObjectPtr<UInputAction> IA_Crouch;
};
