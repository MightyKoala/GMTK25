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
