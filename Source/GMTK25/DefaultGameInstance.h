#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FrameRecording.h"
#include "DefaultGameInstance.generated.h"

UCLASS()
class GMTK25_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void IncreaseDeathCount() { DeathCount++; }
	int GetDeathCount() { return DeathCount; }
	void StoreStartingFrame();
	void ResetToStartingFrame();
	void RecordFrame();
	void StoreRecordedFrames();
	const TArray<PlayerFrameRecording>& GetRecordedPlayerFrames(int index) { return RecordedPlayerFrames[index]; }

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void ResetGameInstance();

	FrameSnapShot RecordingStartSnapshot;
private:
	UPROPERTY(VisibleAnywhere, Category = "Game Stats")
	int DeathCount = 0;
	TArray<TArray<PlayerFrameRecording>> RecordedPlayerFrames;
	TArray<PlayerFrameRecording> CurrentPlayerFrames;
};
