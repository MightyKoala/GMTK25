#include "PlayerSpawnPoint.h"

APlayerSpawnPoint::APlayerSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

