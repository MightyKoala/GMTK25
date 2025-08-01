// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSpawnPoint.h"
#include "DefaultGameInstance.h"

ADefaultGameMode::ADefaultGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	LevelTimer = LevelTime;
}

void ADefaultGameMode::ReloadLevel()
{
	UE_LOG(LogTemp, Warning, TEXT("Gamemode reloading level!"));
	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::OpenLevel(World, FName(World->GetMapName()));
	}
}

void ADefaultGameMode::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay!"));
	Super::BeginPlay();
	LevelTimer = LevelTime;
	PlayBackTimer = 0.f;
	GhostPlayers.Empty();
	PlayBackIndexes.Empty();

	int deathCount = 0;
	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance)
	{
		deathCount = GameInstance->GetDeathCount();
	}

	for (int i = 0; i < deathCount; i++)
	{
		SpawnPlayerReplayCharacter(GetNextSpawnPoint(), FVector::ForwardVector.Rotation());
		UE_LOG(LogTemp, Warning, TEXT("Spawning replay character!"));
	}

	SpawnPlayer();
}

void ADefaultGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("My level timer is: %f"), LevelTimer);
	LevelTimer -= DeltaTime;
	if (LevelTimer <= 0.f)
	{
		//ReloadLevel();
	}

	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GhostPlayers.IsEmpty() && GameInstance)
	{
		PlayBackTimer += DeltaTime;

		for (int ghostIndex = 0; ghostIndex < GhostPlayers.Num(); ghostIndex++)
		{
			if (!IsValid(GhostPlayers[ghostIndex]))
				continue;
			int frameCount = GameInstance->GetRecordedPlayerFrames(ghostIndex).Num();
			int lastPlaybackIndex = PlayBackIndexes[ghostIndex];
			for (int frameIndex = lastPlaybackIndex + 1; frameIndex < frameCount; frameIndex++)
			{
				const PlayerFrameRecording& frame = GameInstance->GetRecordedPlayerFrames(ghostIndex)[frameIndex];
				if (frame.TimeStamp < PlayBackTimer)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Replaying frame %d on ghost!"), frameIndex);
					GhostPlayers[ghostIndex]->SimulateFrame(frame);
					PlayBackIndexes[ghostIndex] = frameIndex;
				}
				else
				{
					break;
				}
			}
		}
	}
}

void ADefaultGameMode::SpawnPlayer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	APlayerCharacter* newPlayer = World->SpawnActor<APlayerCharacter>(PlayerToSpawn, GetNextSpawnPoint(), FVector::ForwardVector.Rotation(), SpawnParams);

	if (newPlayer)
	{
		// Additional initialization logic can go here
		UE_LOG(LogTemp, Warning, TEXT("Spawned player character!"));
	}
}

void ADefaultGameMode::SpawnPlayerReplayCharacter(FVector SpawnLocation, FRotator SpawnRotation)
{
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	APlayerGhostCharacter* ghostPlayer = World->SpawnActor<APlayerGhostCharacter>(PlayerReplayPawn, SpawnLocation, SpawnRotation, SpawnParams);

	if (ghostPlayer)
	{
		// Additional initialization logic can go here
		UE_LOG(LogTemp, Warning, TEXT("Spawned replay character!"));
	}
	GhostPlayers.Add(ghostPlayer);
	PlayBackIndexes.Add(0);
}

FVector ADefaultGameMode::GetNextSpawnPoint()
{
	TArray<AActor*> spawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerSpawnPoint::StaticClass(), spawnPoints);
	FVector spawnPos = FVector::Zero();
	int lowestSpawnOrderIndex = INT_MAX;

	for (AActor* spawnPoint : spawnPoints)
	{
		if (APlayerSpawnPoint* currentSpawnPoint = Cast<APlayerSpawnPoint>(spawnPoint))
		{
			int spawnOrder = currentSpawnPoint->GetSpawnOrderIndex();
			if (spawnOrder < lowestSpawnOrderIndex && !currentSpawnPoint->IsAlreadyUsed())
			{
				lowestSpawnOrderIndex = spawnOrder;
				spawnPos = currentSpawnPoint->GetActorLocation();
				currentSpawnPoint->SetIsUsed(true);
			}
		}
	}
	return spawnPos;
}
