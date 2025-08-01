#include "DefaultGameInstance.h"


void UDefaultGameInstance::RecordFrame(PlayerFrameRecording frame)
{
	CurrentPlayerFrames.Add(frame);
}

void UDefaultGameInstance::StoreRecordedFrames()
{
	RecordedPlayerFrames.Add(CurrentPlayerFrames);
	CurrentPlayerFrames.Empty();
}

void UDefaultGameInstance::ResetGameInstance()
{
	RecordedPlayerFrames.Empty();
	CurrentPlayerFrames.Empty();
	DeathCount = 0;
}
