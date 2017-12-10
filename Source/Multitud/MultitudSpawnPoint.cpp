// Fill out your copyright notice in the Description page of Project Settings.

#include "MultitudSpawnPoint.h"


// Sets default values
AMultitudSpawnPoint::AMultitudSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));    
	SpawnCapsule->SetCollisionProfileName("OverlapAllDynamic");    
	SpawnCapsule->bGenerateOverlapEvents = true;    
	SpawnCapsule->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);

	OnActorBeginOverlap.AddDynamic(this, &AMultitudSpawnPoint::ActorBeginOverlaps);    
	OnActorEndOverlap.AddDynamic(this, &AMultitudSpawnPoint::ActorEndOverlaps);
}

void AMultitudSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMultitudSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SpawnCapsule->UpdateOverlaps();
}

void AMultitudSpawnPoint::OnConstruction(const FTransform& Transform) 
{
	if (Team == ETeam::RED_TEAM) 
	{ 
		SpawnCapsule->ShapeColor = FColor(255, 0, 0); 
	}
	else // (Team == ETeam::BLUE_TEAM)    
	{        
		SpawnCapsule->ShapeColor = FColor(0, 0, 255);    
	} 
}

void AMultitudSpawnPoint::ActorBeginOverlaps(AActor* ThisActor, AActor* OtherActor)
{ 
	if (ROLE_Authority == Role) 
	{ 
		if (OverlappingActors.Find(OtherActor) == INDEX_NONE) 
		{ 
			OverlappingActors.Add(OtherActor); 
		} 
	} 
}

void AMultitudSpawnPoint::ActorEndOverlaps(AActor* ThisActor, AActor* OtherActor)
{ 
	if (ROLE_Authority == Role) 
	{ 
		if (OverlappingActors.Find(OtherActor) != INDEX_NONE) 
		{ 
			OverlappingActors.Remove(OtherActor); 
		} 
	} 
}
