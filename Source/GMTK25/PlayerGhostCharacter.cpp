#include "PlayerGhostCharacter.h"

void APlayerGhostCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive)
		return;

	SetActorRotation(LastDirection.Rotation());
}

void APlayerGhostCharacter::SimulateFrame(const PlayerFrameRecording& frame)
{
	SetActorLocation(frame.Location);
	LastDirection = frame.ForwardVector;
	SetActorRotation(frame.ForwardVector.Rotation());

	if (frame.ShootInput)
	{
		OnShootEvent();
	}
}

void APlayerGhostCharacter::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;
}
