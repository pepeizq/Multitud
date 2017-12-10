// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MultitudGameMode.h"

#include "Multitud.h"
#include "MultitudCharacter.h"
#include "MultitudGameState.h"
#include "MultitudHUD.h"
#include "MultitudPlayerState.h"
#include "MultitudSpawnPoint.h"

#include "Public/EngineUtils.h"
#include "UObject/ConstructorHelpers.h"

bool AMultitudGameMode::bInGameMenu = true;

AMultitudGameMode::AMultitudGameMode() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerStateClass = AMultitudPlayerState::StaticClass();
	HUDClass = AMultitudHUD::StaticClass();
	GameStateClass = AMultitudGameState::StaticClass();

	bReplicates = true;
}

void AMultitudGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		Cast<AMultitudGameState>(GameState)->bInMenu = bInGameMenu;

		for (TActorIterator<AMultitudSpawnPoint> Iter(GetWorld()); Iter; ++Iter)
		{
			if ((*Iter)->Team == ETeam::RED_TEAM)
			{
				RedSpawns.Add(*Iter);
			}
			else
			{
				BlueSpawns.Add(*Iter);
			}
		}

		// Spawn the server        
		APlayerController* thisCont = GetWorld()->GetFirstPlayerController();

		if (thisCont)
		{
			AMultitudCharacter* thisChar = Cast<AMultitudCharacter>(thisCont->GetPawn());
			thisChar->SetTeam(ETeam::BLUE_TEAM);            
			BlueTeam.Add(thisChar);            
			Spawn(thisChar);
		}		
	}
}

void AMultitudGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Quit || EndPlayReason == EEndPlayReason::EndPlayInEditor) 
	{ 
		bInGameMenu = true; 
	}
}

void AMultitudGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Role == ROLE_Authority)
	{
		APlayerController* thisCont = GetWorld()->GetFirstPlayerController();

		if (ToBeSpawned.Num() != 0)
		{
			for (auto charToSpawn : ToBeSpawned)
			{
				Spawn(charToSpawn);
			}
		}

		if (thisCont != nullptr && thisCont->IsInputKeyDown(EKeys::R))
		{ 
			bInGameMenu = false;            
			GetWorld()->ServerTravel(L"/Game/FirstPersonCPP/Maps/FirstPersonExampleMap?Listen");
			Cast<AMultitudGameState>(GameState)->bInMenu = bInGameMenu;
		}
	}
}

void AMultitudGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AMultitudCharacter* Teamless = Cast<AMultitudCharacter>(NewPlayer->GetPawn());
	AMultitudPlayerState* MPlayerState = Cast<AMultitudPlayerState>(NewPlayer->PlayerState);

	if (Teamless != nullptr && MPlayerState != nullptr)
	{
		Teamless->SetMultitudPlayerState(MPlayerState);
	}

	// Assign Team and spawn
	if (Role == ROLE_Authority && Teamless != nullptr)
	{
		if (BlueTeam.Num() > RedTeam.Num())
		{
			RedTeam.Add(Teamless);            
			MPlayerState->Team = ETeam::RED_TEAM;
		}
		else if (BlueTeam.Num() < RedTeam.Num())
		{
			BlueTeam.Add(Teamless);            
			MPlayerState->Team = ETeam::BLUE_TEAM;
		}
		else // Teams are equal
		{
			BlueTeam.Add(Teamless);
			MPlayerState->Team = ETeam::BLUE_TEAM;
		}

		Teamless->CurrentTeam = MPlayerState->Team;        
		Teamless->SetTeam(MPlayerState->Team);
		Spawn(Teamless);
	}
}

void AMultitudGameMode::Spawn(class AMultitudCharacter* Character)
{
	if (Role == ROLE_Authority)
	{
		// Find Spawn point that is not blocked
		AMultitudSpawnPoint* thisSpawn = nullptr;        
		TArray<AMultitudSpawnPoint*>* targetTeam = nullptr;

		if (Character->CurrentTeam == ETeam::BLUE_TEAM)
		{
			targetTeam = &BlueSpawns;
		}
		else
		{
			targetTeam = &RedSpawns;
		}

		for (auto Spawn : (*targetTeam))
		{
			if (!Spawn->GetBlocked())
			{
				// Remove from spawn queue location
				if (ToBeSpawned.Find(Character) != INDEX_NONE)
				{
					ToBeSpawned.Remove(Character);
				}

				// Otherwise set actor location
				Character->SetActorLocation(Spawn->GetActorLocation());
				Spawn->UpdateOverlaps();
				return;
			}
		}

		if (ToBeSpawned.Find(Character) == INDEX_NONE)
		{
			ToBeSpawned.Add(Character); 
		}
	}
}

void AMultitudGameMode::Respawn(class AMultitudCharacter* Character)
{
	if (Role == ROLE_Authority)
	{
		AController* thisPC = Character->GetController();        
		
		Character->DetachFromControllerPendingDestroy();

		AMultitudCharacter* newChar = Cast<AMultitudCharacter>(GetWorld()->SpawnActor(DefaultPawnClass));

		if (newChar)
		{
			thisPC->Possess(newChar);

			AMultitudPlayerState* thisPS = Cast<AMultitudPlayerState>(newChar->GetController()->PlayerState);

			newChar->CurrentTeam = thisPS->Team;
			newChar->SetMultitudPlayerState(thisPS);

			Spawn(newChar);

			newChar->SetTeam(newChar->GetMultitudPlayerState()->Team);
		}
	}
}