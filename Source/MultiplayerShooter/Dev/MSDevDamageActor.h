// Test Multiplayer Shooter Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MSDevDamageActor.generated.h"



UCLASS()
class MULTIPLAYERSHOOTER_API AMSDevDamageActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AMSDevDamageActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor SphereColor = FColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DoFullDamage = false;


protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;
};
