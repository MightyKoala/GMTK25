#include "PlayerGhostCharacter.h"

void APlayerGhostCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerGhostCharacter::SimulateFrame(const PlayerFrameRecording& frame)
{
	//MoveAct(frame.MovementInput);
	SetActorLocation(frame.Location);
	SetActorRotation(frame.ForwardVector.Rotation());

	if (frame.ShootInput)
	{
		OnShootEvent();
	}
}
