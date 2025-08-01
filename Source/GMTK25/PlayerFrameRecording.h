#pragma once

#include "CoreMinimal.h"

struct PlayerFrameRecording
{
public:
	FVector Location;
	double TimeStamp;
	FVector ForwardVector;
	bool ShootInput;

	void Reset()
	{
		Location = FVector::Zero();
		TimeStamp = 0;
		ForwardVector = FVector::Zero();
		ShootInput = false;
	}
};
