// Test Multiplayer Shooter Game. All Rights Reserved.


#include "MSDevDamageActor.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

AMSDevDamageActor::AMSDevDamageActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);
}

void AMSDevDamageActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMSDevDamageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 24, SphereColor);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), Radius, nullptr, {}, this, nullptr, DoFullDamage);
}
