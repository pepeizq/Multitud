
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MultitudGameState.generated.h"

UCLASS()
class MULTITUD_API AMultitudGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:    
	
	AMultitudGameState();

	UPROPERTY(Replicated)    
	bool bInMenu;
	
};
