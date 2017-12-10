
#include "MultitudGameState.h"

#include "Net/UnrealNetwork.h" 

AMultitudGameState::AMultitudGameState() 
{
	bInMenu = false; 
}

void AMultitudGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultitudGameState, bInMenu);
}

