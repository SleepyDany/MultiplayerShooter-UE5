// Test Multiplayer Shooter Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MSBaseWeapon.generated.h"

class USkeletalMeshComponent;

USTRUCT(BlueprintType)
struct FWeaponSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxDistance = 4000.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 8.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FireRate = 0.1f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Spread = 1.0f;
};

UCLASS()
class MULTIPLAYERSHOOTER_API AMSBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AMSBaseWeapon();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void StartFire();
	virtual void StopFire();

	virtual void Reload();
	virtual void Aim();

	FORCEINLINE bool CanFire() const { return CurrentAmmo > 0 && !bIsOnReload; }
	FORCEINLINE bool IsOnAim() const { return bIsOnAim; }
	FORCEINLINE bool IsOnReload() const { return bIsOnReload; }

	FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmo; }
	FORCEINLINE int32 GetAllAmmo() const { return AllAmmo; }

protected:
	virtual void BeginPlay() override;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName MuzzleSocketName = "MuzzleSocket";
    FWeaponSettings WeaponSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HolderAmmo = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_AmmoChanged)
	int32 CurrentAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_AmmoChanged)
	int32 AllAmmo = 120;

    // TODO: rework multiplayer
	UFUNCTION()
	void OnRep_AmmoChanged();

	UPROPERTY(VisibleAnywhere)
	bool bIsOnAim = false;
	UPROPERTY(VisibleAnywhere)
	bool bIsOnReload = false;

protected:
	TObjectPtr<APlayerController> GetPlayerController() const;

	bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;
    
	void MakeShot();
	FVector GetMuzzleWorldLocation() const;
	bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;
	void DrawLineTrace(const FHitResult& FHitResult, const FVector& TraceStart, const FVector& TraceEnd) const;

	void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd) const;
	void MakeDamage(const FHitResult& HitResult);

private:
	FTimerHandle FireTimer;
};
