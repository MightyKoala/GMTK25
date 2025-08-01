#include "DefaultGameInstance.h"


void UDefaultGameInstance::RecordFrame(PlayerFrameRecording frame)
{
	CurrentPlayerFrames.Add(frame);
}

void UDefaultGameInstance::StoreRecordedFrames()
{
	RecordedPlayerFrames.Empty();
	RecordedPlayerFrames = CurrentPlayerFrames;
	CurrentPlayerFrames.Empty();
}
