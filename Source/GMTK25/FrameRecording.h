#pragma once

#include "CoreMinimal.h"

class APlayerCharacter;

struct PlayerFrameRecording
{
public:
	FVector Location;
	float TimeStamp;
	FVector ForwardDirection;
	bool ShootInput;
};

struct GhostFrame
{
	int ReplayIndex;
	bool IsAlive;
};

struct EnemyFrame
{
	FVector ForwardDirection;
	float FireRateTimer;
	APlayerCharacter* Target;
	bool IsAlive;
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
};
