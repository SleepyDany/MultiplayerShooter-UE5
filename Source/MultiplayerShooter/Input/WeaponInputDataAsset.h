// Test Multiplayer Shooter Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputDataAsset.h"
#include "WeaponInputDataAsset.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API UWeaponInputDataAsset : public UInputDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TObjectPtr<UInputAction> IA_Fire;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TObjectPtr<UInputAction> IA_Aim;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TObjectPtr<UInputAction> IA_Reload;
};
