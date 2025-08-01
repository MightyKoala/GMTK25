// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameMode.h"

ADefaultGameMode::ADefaultGameMode()
{
	LevelTimer = LevelTime;
}

void ADefaultGameMode::RecordFrame(APlayerCharacter::PlayerFrameRecording frame)
{
	RecordedPlayerFrames.Add(frame);
}

void ADefaultGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (restarted == false)
	{
		LevelTimer -= DeltaTime;
		if (LevelTimer <= 0.f)
		{
			restarted = true;
		}
	}

	if (restarted)
	{
		for (APlayerCharacter::PlayerFrameRecording& frame : RecordedPlayerFrames)
		{
			if (frame.TimeStamp < playBackTimer)
			{

			}
			else
			{
				break;
			}
		}
	}
}
