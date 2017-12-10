// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultitudGameMode.h"
#include "Components/CapsuleComponent.h"
#include "MultitudSpawnPoint.generated.h"

UCLASS()
class MULTITUD_API AMultitudSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	

	AMultitudSpawnPoint();

	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()        
	void ActorBeginOverlaps(AActor* ThisActor, AActor* OtherActor);

	UFUNCTION()    
	void ActorEndOverlaps(AActor* ThisActor, AActor* OtherActor);

	bool GetBlocked() 
	{ 
		return OverlappingActors.Num() != 0; 
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)    
	ETeam Team;

protected:

	virtual void BeginPlay() override;

private:

	UCapsuleComponent* SpawnCapsule;

	TArray<class AActor*> OverlappingActors;
	
};
