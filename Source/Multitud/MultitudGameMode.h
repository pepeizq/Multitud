
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultitudGameMode.generated.h"

UENUM(BlueprintType) 
enum class ETeam : uint8 
{ 
	BLUE_TEAM, 
	RED_TEAM 
};

UCLASS(minimalapi)
class AMultitudGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AMultitudGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer) override; 
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Respawn(class AMultitudCharacter* Character);   
	
	void Spawn(class AMultitudCharacter* Character);

private:

	TArray<class AMultitudCharacter*> RedTeam;    
	
	TArray<class AMultitudCharacter*> BlueTeam;

	TArray<class AMultitudSpawnPoint*> RedSpawns;    
	
	TArray<class AMultitudSpawnPoint*> BlueSpawns;    
	
	TArray<class AMultitudCharacter*> ToBeSpawned;

	bool bGameStarted;    
	
	static bool bInGameMenu;

};



