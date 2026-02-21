#pragma once

#include "CoreMinimal.h"

class ACharacterBase;

struct PlayerFrameRecording
{
	FVector Location;
	FVector ForwardDirection;
	bool ShootInput;
};

struct GhostFrame
{
	int ReplayIndex;
	bool IsAlive;
};

struct EnemyFrameRecording
{
	FVector Location;
	FVector ForwardDirection;
	float FireRateTimer;
	ACharacterBase* Target;
	int Health;
};

struct BulletState
{
	FVector Location;
};

struct ObjectiveState
{
	bool IsActivated;
	bool IsEnabled;
	bool IsAlive;
};

struct FrameSnapShot
{
	PlayerFrameRecording PlayerFrame;
	double TimeStamp;
	TArray<int> PlayBackIndexes;
	TArray<EnemyFrameRecording> EnemyFrames;
};
