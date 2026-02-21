#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "DefaultPlayerController.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	PlayerCamera->SetupAttachment(CameraSpringArm);
}

bool APlayerCharacter::GetShotInput()
{
	if (ADefaultPlayerController* playerController = Cast<ADefaultPlayerController>(Controller))
	{
		return playerController->ShotInput;
	}
	return false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameAndUI InputMode;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive)
		return;

	if (ADefaultPlayerController* playerController = Cast<ADefaultPlayerController>(Controller))
	{
		playerController->ShotInput = false;
		playerController->SetAudioListenerOverride(nullptr, GetActorLocation(), FVector(1.f, 0.f, 0.f).Rotation());
		FVector OutLoc, OutForward, OutRight;
		playerController->GetAudioListenerPosition(OutLoc, OutForward, OutRight);

		float MouseX, MouseY;
		playerController->GetMousePosition(MouseX, MouseY);

		FVector WorldLocation, WorldDirection;
		playerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

		float ZDifference = GetActorLocation().Z - WorldLocation.Z;
		float Scale = ZDifference / WorldDirection.Z;

		FVector TargetPosition = WorldLocation + (WorldDirection * Scale);

		FVector playerPos = GetActorLocation();
		FVector directionToMouse = TargetPosition - playerPos;
		directionToMouse.Z = 0.0f;
		FRotator newRotation = directionToMouse.Rotation();
		SetActorRotation(newRotation);
		UE_LOG(LogTemp, Warning, TEXT("Set Player Rotation: %s"), *newRotation.Vector().ToString());
		mLastForward = newRotation.Vector();
	}
}
