#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerFrameRecording.h"
#include "DefaultGameInstance.generated.h"

UCLASS()
class GMTK25_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void IncreaseDeathCount() { DeathCount++; }
	int GetDeathCount() { return DeathCount; }
	void RecordFrame(PlayerFrameRecording frame);
	void StoreRecordedFrames();
	TArray<PlayerFrameRecording>& GetRecordedPlayerFrames() { return RecordedPlayerFrames; }

private:
	UPROPERTY(VisibleAnywhere, Category = "Game Stats")
	int DeathCount = 0;
	TArray<PlayerFrameRecording> RecordedPlayerFrames;
	TArray<PlayerFrameRecording> CurrentPlayerFrames;
};
