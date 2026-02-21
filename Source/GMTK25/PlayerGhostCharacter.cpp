#include "PlayerGhostCharacter.h"
#include "Kismet/GameplayStatics.h"

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
	LastDirection = frame.ForwardDirection;
	SetActorRotation(frame.ForwardDirection.Rotation());
	APlayerController* controller = UGameplayStatics::GetPlayerController(this->GetWorld(), 0);
	controller->SetControlRotation(frame.ForwardDirection.Rotation());

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
