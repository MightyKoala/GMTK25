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
		UE_LOG(LogTemp, Warning, TEXT("Shot fired! on frame %f"), frame.TimeStamp)
		ShootAct(frame.Location, frame.ForwardVector);
	}
}
