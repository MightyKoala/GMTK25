// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultPlayerController.h"

ADefaultPlayerController::ADefaultPlayerController()
{
}

void ADefaultPlayerController::BeginPlay()
{
}

void ADefaultPlayerController::SetupInputComponent()
{
}

void ADefaultPlayerController::Tick(float DeltaTime)
{
	FVector worldPosition, worldDirection;
	DeprojectMousePositionToWorld(worldPosition, worldDirection);
}
