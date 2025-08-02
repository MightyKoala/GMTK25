#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissionObjective.generated.h"

UCLASS()
class GMTK25_API AMissionObjective : public AActor
{
	GENERATED_BODY()
	
public:	
	AMissionObjective();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
