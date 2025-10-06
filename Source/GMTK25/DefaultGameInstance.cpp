#include "DefaultGameInstance.h"
#include "CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "DefaultGameMode.h"

void UDefaultGameInstance::StoreStartingFrame()
{
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
		return;

	TArray<AActor*> players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), players);
	for (AActor* actor : players)
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(actor))
		{
			PlayerFrameRecording currentFrame;
			currentFrame.Location = player->GetActorLocation();
			currentFrame.ForwardDirection = player->GetActorForwardVector();
			currentFrame.ShootInput = player->GetShotInput();
			currentFrame.TimeStamp = GameMode->LevelTimer;
			RecordingStartSnapshot.PlayerFrame = currentFrame;
			RecordingStartSnapshot.TimeStamp = GameMode->LevelTimer;
		}
	}
}

void UDefaultGameInstance::ResetToStartingFrame()
{
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
		return;

	TArray<AActor*> players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), players);
	for (AActor* actor : players)
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(actor))
		{
			player->SetActorLocation(RecordingStartSnapshot.PlayerFrame.Location);
		}
	}
	GameMode->LevelTimer = RecordingStartSnapshot.TimeStamp;
	float Yaw = FMath::Atan2(RecordingStartSnapshot.PlayerFrame.ForwardDirection.Y, RecordingStartSnapshot.PlayerFrame.ForwardDirection.X) * (180.0f / PI);
	GameMode->SpawnPlayerReplayCharacter(RecordingStartSnapshot.PlayerFrame.Location, FRotator::MakeFromEuler({ 0.f, Yaw, 0.f }));
	StoreRecordedFrames();
}

void UDefaultGameInstance::RecordFrame()
{
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
		return;

	TArray<AActor*> players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), players);
	for (AActor* actor : players)
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(actor))
		{
			PlayerFrameRecording currentFrame;
			currentFrame.Location = player->GetActorLocation();
			currentFrame.ForwardDirection = player->GetActorForwardVector();
			currentFrame.ShootInput = player->GetShotInput();
			currentFrame.TimeStamp = GameMode->LevelTimer;
			CurrentPlayerFrames.Add(currentFrame);
		}
	}
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
