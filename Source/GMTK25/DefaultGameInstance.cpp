#include "DefaultGameInstance.h"
#include "CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "EnemyCharacter.h"
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
			PlayerFrameRecording playerFrame;
			playerFrame.Location = player->GetActorLocation();
			playerFrame.ForwardDirection = player->GetActorForwardVector();
			playerFrame.ShootInput = player->GetShotInput();
			RecordingStartSnapshot.PlayerFrame = playerFrame;
			RecordingStartSnapshot.TimeStamp = GameMode->LevelTimer;
			RecordingStartSnapshot.PlayBackIndexes = GameMode->PlayBackIndexes;
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

	TArray<AActor*> enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), players);
	for (AActor* actor : enemies)
	{
		if (AEnemyCharacter* enemy = Cast<AEnemyCharacter>(actor))
		{
			//TODO: Use proper index and find from actor array
			enemy->SetActorLocation(RecordingStartSnapshot.EnemyFrames[0].Location);
			enemy->SetActorRotation(RecordingStartSnapshot.EnemyFrames[0].ForwardDirection.Rotation());
			enemy->SetHealth(RecordingStartSnapshot.EnemyFrames[0].Health);
			enemy->FireRateTimer = RecordingStartSnapshot.EnemyFrames[0].FireRateTimer;
			enemy->TargetPlayer = RecordingStartSnapshot.EnemyFrames[0].Target;
		}
	}
	GameMode->LevelTimer = RecordingStartSnapshot.TimeStamp;
	GameMode->PlayBackIndexes = RecordingStartSnapshot.PlayBackIndexes;
	float Yaw = FMath::Atan2(RecordingStartSnapshot.PlayerFrame.ForwardDirection.Y, RecordingStartSnapshot.PlayerFrame.ForwardDirection.X) * (180.0f / PI);
	GameMode->SpawnPlayerReplayCharacter(RecordingStartSnapshot.PlayerFrame.Location, FRotator::MakeFromEuler({ 0.f, Yaw, 0.f }));
	StoreRecordedFrames();
}

void UDefaultGameInstance::RecordFrame()
{
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
		return;

	FrameSnapShot currentFrame;
	currentFrame.TimeStamp = GameMode->LevelTimer;

	TArray<AActor*> players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), players);
	for (AActor* actor : players)
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(actor))
		{
			PlayerFrameRecording playerFrame;
			playerFrame.Location = player->GetActorLocation();
			playerFrame.ForwardDirection = player->mLastForward;// ->GetActorForwardVector();
			playerFrame.ShootInput = player->GetShotInput();
			currentFrame.PlayerFrame = playerFrame;
			UE_LOG(LogTemp, Warning, TEXT("Current Frame Direction: %s"), *playerFrame.ForwardDirection.ToString());
			UE_LOG(LogTemp, Warning, TEXT("Current Forward Vector: %s"), *player->GetActorForwardVector().ToString());
			UE_LOG(LogTemp, Warning, TEXT("Current Rotation Vector: %s"), *player->GetActorRotation().Vector().ToString());
			UE_LOG(LogTemp, Warning, TEXT("Current World Rotation: %s"), *player->ActorToWorld().Rotator().Vector().ToString());
		}
	}

	TArray<AActor*> enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), enemies);
	for (AActor* enemyActor : enemies)
	{
		if (AEnemyCharacter* enemy = Cast<AEnemyCharacter>(enemyActor))
		{
			EnemyFrameRecording enemyFrame;
			enemyFrame.Location = enemy->GetActorLocation();
			enemyFrame.ForwardDirection = enemy->GetActorForwardVector();
			enemyFrame.FireRateTimer = enemy->FireRateTimer;
			enemyFrame.Target = enemy->TargetPlayer;
			enemyFrame.Health = 1;//enemy->GetHealth();
			currentFrame.EnemyFrames.Add(enemyFrame);
		}
	}

	CurrentFrames.Add(currentFrame);
}

void UDefaultGameInstance::StoreRecordedFrames()
{
	RecordedFrames.Add(CurrentFrames);
	CurrentFrames.Empty();
}

void UDefaultGameInstance::ResetGameInstance()
{
	RecordedFrames.Empty();
	CurrentFrames.Empty();
	DeathCount = 0;
}
