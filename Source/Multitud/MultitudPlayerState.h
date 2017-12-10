// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MultitudGameMode.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MultitudPlayerState.generated.h"

UCLASS()
class MULTITUD_API AMultitudPlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(Replicated)
	float Health;
	
	UPROPERTY(Replicated)    
	uint8 Deaths;

	UPROPERTY(Replicated)    
	ETeam Team;
};
