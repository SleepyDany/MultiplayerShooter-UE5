// Test Multiplayer Shooter Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MSWeaponComponent.generated.h"

class AMSBaseWeapon;

DECLARE_MULTICAST_DELEGATE(FOnAmmoChanged)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UMSWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMSWeaponComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AMSBaseWeapon* GetWeapon() const { return Weapon.Get(); }
	void SpawnWeapon();


    // TODO: Refactor multiplayer

	UFUNCTION()
	void Aim() const;
	UFUNCTION(Server, Reliable)
	void StartFire();
	UFUNCTION(Server, Reliable)
	void StopFire();
	UFUNCTION(Server, Reliable)
	void Reload();

	FOnAmmoChanged OnAmmoChanged;

	int32 GetCurrentAmmo() const;
	int32 GetAllAmmo() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AMSBaseWeapon> WeaponClass;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponAttachPointName = "WeaponSocket";

private:
	UPROPERTY(Replicated)
	TObjectPtr<AMSBaseWeapon> Weapon = nullptr;
};
