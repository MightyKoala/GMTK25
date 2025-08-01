#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerSpawnPoint.generated.h"

UCLASS()
class GMTK25_API APlayerSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APlayerSpawnPoint();
	virtual void Tick(float DeltaTime) override;
	int GetSpawnOrderIndex() { return SpawnOrderIndex; }
	int IsAlreadyUsed() { return Used; }
	void SetIsUsed(bool used) { Used = used; }

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Logic")
	int SpawnOrderIndex = 0;
	bool Used = false;
};
