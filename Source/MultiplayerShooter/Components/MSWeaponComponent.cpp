// Test Multiplayer Shooter Game. All Rights Reserved.


#include "MSWeaponComponent.h"

#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "MSBaseWeapon.h"
#include "MSBaseCharacter.h"

UMSWeaponComponent::UMSWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMSWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMSWeaponComponent, Weapon);
}

void UMSWeaponComponent::StartFire_Implementation()
{
	if (!Weapon)
		return;

	Weapon->StartFire();
	OnAmmoChanged.Broadcast();
}

void UMSWeaponComponent::StopFire_Implementation()
{ 
	if (!Weapon) 
		return; 
	
	Weapon->StopFire();
	OnAmmoChanged.Broadcast();
}

void UMSWeaponComponent::Reload_Implementation()
{
	if (!Weapon)
		return;

	Weapon->Reload();
	OnAmmoChanged.Broadcast();
}

void UMSWeaponComponent::Aim() const
{
	if (!Weapon) { return; }

	Weapon->Aim();
}

void UMSWeaponComponent::SpawnWeapon()
{
	const auto Owner = GetOwner<AMSBaseCharacter>();
	if (!Owner || !GetWorld()) { return; }

	Weapon = GetWorld()->SpawnActor<AMSBaseWeapon>(WeaponClass);
	if (!Weapon) { return; }

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	Weapon->AttachToComponent(Owner->GetMesh(), AttachmentRules, WeaponAttachPointName);
	Weapon->SetOwner(Owner);

	OnAmmoChanged.Broadcast();
}

int32 UMSWeaponComponent::GetCurrentAmmo() const
{
	return !Weapon ? 0 : Weapon->GetCurrentAmmo();
}

int32 UMSWeaponComponent::GetAllAmmo() const
{ 
	return !Weapon ? 0 : Weapon->GetAllAmmo();
}
