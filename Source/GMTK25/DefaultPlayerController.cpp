#include "DefaultPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DefaultGameMode.h"
#include "PlayerCharacter.h"

ADefaultPlayerController::ADefaultPlayerController()
{
}

void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
	SetViewTarget(GetPawn());
	if (UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		subSystem->AddMappingContext(InputContext, 0);
	}
}

void ADefaultPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		enhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ADefaultPlayerController::Move);
		enhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ADefaultPlayerController::Shoot);
		enhancedInputComponent->BindAction(RecordAction, ETriggerEvent::Triggered, this, &ADefaultPlayerController::StartRecording);
		enhancedInputComponent->BindAction(EscapeMenuAction, ETriggerEvent::Started, this, &ADefaultPlayerController::TogglePauseMenu);
	}
}

void ADefaultPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADefaultPlayerController::Move(const FInputActionValue& value)
{
	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetCharacter()))
		player->MoveAct(value.Get<FVector2D>());
}

void ADefaultPlayerController::Shoot()
{
	APlayerCharacter* player = Cast<APlayerCharacter>(GetCharacter());
	if (!player)
		return;
	if (!player->IsAlive)
		return;
	ShotInput = true;
	player->OnShootEvent();
}

void ADefaultPlayerController::StartRecording()
{
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode)
	{
		GameMode->StartRecordingFrames();
	}
}

void ADefaultPlayerController::TogglePauseMenu()
{
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode)
	{
		GameMode->TogglePauseScreenVisibility();
	}
}
