#include "MissionObjective.h"

AMissionObjective::AMissionObjective()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMissionObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMissionObjective::BeginPlay()
{
	Super::BeginPlay();
}

