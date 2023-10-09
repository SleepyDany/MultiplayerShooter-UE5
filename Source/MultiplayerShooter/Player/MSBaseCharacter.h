// Test Multiplayer Shooter Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "MSBaseCharacter.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UWeaponInputDataAsset;
class UCommonInputDataAsset;
class UCameraComponent;
class USpringArmComponent;
class UMSHealthComponent;
class UTextRenderComponent;
class UMSWeaponComponent;
class UMSCharacterMovementComponent;

UCLASS()
class MULTIPLAYERSHOOTER_API AMSBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMSBaseCharacter(const FObjectInitializer& ObjectInitializer);

    virtual void PostInitializeComponents() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	bool IsWalking() const;
	UFUNCTION()
	void OnRep_HealthChanged();
	UFUNCTION()
	void OnRep_AmmoChanged();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UMSHealthComponent> HealthComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UMSWeaponComponent> WeaponComponent;

    // TODO: rework with UWidgetComponent
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UTextRenderComponent> WeaponTextComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UTextRenderComponent> HealthTextComponent;

protected:
    virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Settings|Movement")
	float GetMovementDirection() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Input")
    TObjectPtr<UCommonInputDataAsset> CommonInputDataAsset;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Input")
    TObjectPtr<UInputMappingContext> CommonInputMapping;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Input")
    TObjectPtr<UWeaponInputDataAsset> WeaponInputDataAsset;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Input")
    TObjectPtr<UInputMappingContext> WeaponInputMapping;
    
private:
    void Look(const FInputActionValue& InputActionValue);
    void Move(const FInputActionValue& InputActionValue);
    void Walk(const FInputActionValue& InputActionValue, bool bIntendToWalk);

	bool bIsWalking = false;

    void StartFire(const FInputActionValue& InputActionValue);
    void StopFire(const FInputActionValue& InputActionValue);
    void Aim(const FInputActionValue& InputActionValue, bool bIntendToAim);
    void Reload(const FInputActionValue& InputActionValue);

    bool bIsAiming = false;

	void OnDeath();
	void OnHealthChanged();

	void OnAmmoChanged() const;
};
