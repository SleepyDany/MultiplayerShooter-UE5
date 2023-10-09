// Test Multiplayer Shooter Game. All Rights Reserved.


#include "MSBaseWeapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"

#include "MSBaseCharacter.h"
#include "Engine/DamageEvents.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, Log, All)

AMSBaseWeapon::AMSBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh.Get());

	CurrentAmmo = HolderAmmo;
	bReplicates = true;
}

void AMSBaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentAmmo);
	DOREPLIFETIME(ThisClass, AllAmmo);
}

void AMSBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// TODO: refactor multiplayer shooting
void AMSBaseWeapon::StartFire()
{
	if (GetWorld() && CanFire())
	{
	    MakeShot();
	    GetWorldTimerManager().SetTimer(FireTimer, this, &AMSBaseWeapon::MakeShot, WeaponSettings.FireRate, true);
	}
}

void AMSBaseWeapon::StopFire()
{
    if (GetWorld())
    {
        GetWorldTimerManager().ClearTimer(FireTimer);
    }
}

void AMSBaseWeapon::MakeShot()
{
	if (!GetWorld()) { return; }

	if (!CanFire())
	{
		StopFire();
		return;
	}

	--CurrentAmmo;
	OnRep_AmmoChanged();

	FVector TraceStart, TraceEnd;
	if (!GetTraceData(TraceStart, TraceEnd)) { return; }

	FHitResult HitResult;
	MakeHit(HitResult, TraceStart, TraceEnd);
	DrawLineTrace(HitResult, TraceStart, TraceEnd);

	if (HitResult.bBlockingHit)
	{
		MakeDamage(HitResult);
	}
}

void AMSBaseWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd) const
{
	if (!GetWorld()) { return; }

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

    // TODO: Shooting collision channel
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
}

void AMSBaseWeapon::MakeDamage(const FHitResult& HitResult)
{
	const TObjectPtr<AActor> DamagedActor = HitResult.GetActor();
	if (!DamagedActor) { return; }

	DamagedActor->TakeDamage(WeaponSettings.Damage, FDamageEvent(), GetPlayerController().Get(), this);
}

// TODO: rework
void AMSBaseWeapon::OnRep_AmmoChanged()
{
    // Server and Client
	const auto ActorOwner = GetOwner<AMSBaseCharacter>();
	if (!ActorOwner) { return; }

	// ActorOwner->OnRep_AmmoChanged();
}

void AMSBaseWeapon::Reload()
{
	if (FireTimer.IsValid())
	{
		StopFire();
	}

	if (CurrentAmmo < HolderAmmo && AllAmmo > 0)
	{
		const int32 AvailableAmmo = AllAmmo >= HolderAmmo - CurrentAmmo ? HolderAmmo - CurrentAmmo : AllAmmo;
		CurrentAmmo += AvailableAmmo;

		AllAmmo -= AvailableAmmo;
	}
}


void AMSBaseWeapon::Aim()
{
}

TObjectPtr<APlayerController> AMSBaseWeapon::GetPlayerController() const
{
	if (const auto Player = GetOwner<APawn>())
	{
	    return Player->GetController<APlayerController>();
	}

	return nullptr;
}

bool AMSBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const
{
	if (const auto& Controller = GetPlayerController())
	{
	   Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	    return true;
	}
    
	return false;
}

FVector AMSBaseWeapon::GetMuzzleWorldLocation() const
{
	return WeaponMesh ? WeaponMesh->GetSocketLocation(MuzzleSocketName) : FVector(0.f);
}

bool AMSBaseWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
	FVector ViewLocation;
	FRotator ViewRotation;
    
	if (!GetPlayerViewPoint(ViewLocation, ViewRotation)) { return false; }

	TraceStart = ViewLocation;
	const float HalfRadSpread = FMath::DegreesToRadians(WeaponSettings.Spread);
	const FVector TraceDirection = FMath::VRandCone(ViewRotation.Vector(), HalfRadSpread);
    
	TraceEnd = TraceStart + TraceDirection * WeaponSettings.MaxDistance;
	
	return true;
}

void AMSBaseWeapon::DrawLineTrace(const FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd) const
{
    if (!GetWorld()) { return; }
    
	if (HitResult.bBlockingHit)
	{
		DrawDebugLine(GetWorld(), TraceStart, HitResult.ImpactPoint, FColor::Orange, false, 3.0f, 0, 2.0f);
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.0f, 24, FColor::Red, false, 3.0f, 0, 2.0f);
	}
	else
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Orange, false, 3.0f, 0, 2.0f);
	}
}
