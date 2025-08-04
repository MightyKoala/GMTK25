// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultPlayerController.h"

ADefaultPlayerController::ADefaultPlayerController() : APlayerController()
{
}

void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	SetInputMode(InputMode);
}

void ADefaultPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ADefaultPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
