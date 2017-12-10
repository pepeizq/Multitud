// Fill out your copyright notice in the Description page of Project Settings.

#include "MultitudPlayerState.h"

#include "Net/UnrealNetwork.h"

AMultitudPlayerState::AMultitudPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Health = 100.0f;    
	Deaths = 0;    
	Team = ETeam::BLUE_TEAM;
}

void AMultitudPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultitudPlayerState, Health);    
	DOREPLIFETIME(AMultitudPlayerState, Deaths);    
	DOREPLIFETIME(AMultitudPlayerState, Team);
}

