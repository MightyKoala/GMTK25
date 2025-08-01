#include "PlayerGhostCharacter.h"

void APlayerGhostCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerGhostCharacter::SimulateFrame(const PlayerFrameRecording& frame)
{
	//Tick(frame.DeltaTime);

	//MoveAct(frame.MovementInput);
	SetActorLocation(frame.Location);
	SetActorRotation(frame.ForwardVector.Rotation());

	if (frame.ShootInput)
	{
		ShootAct(frame.Location, frame.ForwardVector);
	}
}
