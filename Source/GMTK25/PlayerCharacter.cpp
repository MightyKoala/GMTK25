#include "PlayerCharacter.h"
#include "DefaultGameMode.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	PlayerCamera->SetupAttachment(CameraSpringArm);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();		FInputModeGameAndUI InputMode;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive)
		return;

	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode && GameMode->LevelStartTimer > 0.f)
	{
		return;
	}
	
	if (ADefaultPlayerController* playerController = Cast<ADefaultPlayerController>(Controller))
	{
		playerController->SetAudioListenerOverride(nullptr, GetActorLocation(), FVector(1.f, 0.f, 0.f).Rotation());
		FVector OutLoc, OutForward, OutRight;
		playerController->GetAudioListenerPosition(OutLoc, OutForward, OutRight);

		if (GameMode && GameMode->IsLevelOver())
		{
			DisableInput(playerController);
			return;
		}

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
	}

	_CurrentFrame.Location = GetActorLocation();
	_CurrentFrame.ForwardVector = GetActorForwardVector();

	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameMode && GameInstance && GameMode->LevelTime > 0.f)
	{
		_CurrentFrame.TimeStamp = GameMode->LevelTime - GameMode->LevelTime;
		GameInstance->RecordFrame(_CurrentFrame);
	}
	_CurrentFrame.Reset();
}
