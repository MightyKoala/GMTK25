#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "PlayerFrameRecording.h"
#include "PlayerGhostCharacter.generated.h"

UCLASS()
class GMTK25_API APlayerGhostCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	void SimulateFrame(const PlayerFrameRecording& frame);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Shoot")
	void OnShootEvent();

	UFUNCTION(BlueprintImplementableEvent, Category = "Death")
	void OnDeathEvent();

protected:
	FVector LastDirection;
};
