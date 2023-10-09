// Test Multiplayer Shooter Game. All Rights Reserved.


#include "MSHealthComponent.h"

#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "MSBaseCharacter.h"
#include "MSGameModeBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, Log, All)

UMSHealthComponent::UMSHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	Health = MaxHealth;
	OnHealthChanged.Broadcast();

	if (GetOwner())
	{
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UMSHealthComponent::OnTakeAnyDamage);
	}
}

void UMSHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Health);
}

void UMSHealthComponent::OnRep_HealthChanged()
{
	const auto Owner = Cast<AMSBaseCharacter>(GetOwner());
	Owner->OnRep_HealthChanged();
}

void UMSHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || IsDead())
		return;
	
	SetHealth(Health - Damage);

	if (IsDead())
	{
		Killed(InstigatedBy);
		OnDeath.Broadcast();
	}
}

void UMSHealthComponent::SetHealth(float HealthValue)
{
	Health = FMath::Clamp(HealthValue, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast();
}

void UMSHealthComponent::Killed(AController* KillerController)
{
	if (!GetWorld()) { return; }

	const auto GameMode = GetWorld()->GetAuthGameMode<AMSGameModeBase>();
	if (!GameMode) { return; }

	const auto Player = Cast<APawn>(GetOwner());
	const auto VictimController = Player ? Player->Controller : nullptr;

	GameMode->Killed(KillerController, VictimController);
}
