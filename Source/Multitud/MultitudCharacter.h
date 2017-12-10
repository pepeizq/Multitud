// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultitudPlayerState.h"
#include "MultitudCharacter.generated.h"

class UInputComponent;

UCLASS(config = Game)
class AMultitudCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* FP_Mesh;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Gun mesh: 3rd person view (seen only by others) */ 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh) 
	class USkeletalMeshComponent* TP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;

public:
	AMultitudCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** Sound to play each time we fire */ 
	UPROPERTY(EditAnywhere, Category = Gameplay) 
	class USoundBase* PainSound;

	/** 3rd person anim montage asset for gun shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* TP_FireAnimation;

	/** 1st person anim montage asset for gun shot */ 
	UPROPERTY(EditAnywhere, Category = Gameplay) 
	class UAnimMontage* FP_FireAnimation;

	/** particle system for 1st person gun shot effect */ 
	UPROPERTY(EditAnywhere, Category = Gameplay) 
	class UParticleSystemComponent* FP_GunShotParticle;

	/** particle system for 3rd person gun shot effect */ 
	UPROPERTY(EditAnywhere, Category = Gameplay) 
	class UParticleSystemComponent* TP_GunShotParticle;

	/** particle system that will represent a bullet */ 
	UPROPERTY(EditAnywhere, Category = Gameplay) 
	class UParticleSystemComponent* BulletParticle;

	UPROPERTY(EditAnywhere, Category = Gameplay) 
	class UForceFeedbackEffect* HitSuccessFeedback;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Team)    
	ETeam CurrentTeam;

	class AMultitudPlayerState* GetMultitudPlayerState();    

	void SetMultitudPlayerState(class AMultitudPlayerState* newPS);  

	void Respawn();

protected:

	virtual void BeginPlay();

	void OnFire();

	void MoveForward(float Val);

	void MoveRight(float Val);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	void Fire(const FVector pos, const FVector dir);

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	class UMaterialInstanceDynamic* DynamicMat;

	class AMultitudPlayerState* MultitudPlayerState;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PossessedBy(AController* NewController) override;
	

/** REMOTE PROCEDURE CALLS --------------------------------------------------------*/

public:

	// Set's team colour    
	UFUNCTION(NetMultiCast, Reliable)
	void SetTeam(ETeam NewTeam);

private:

	// Peform fire action on the server    
	UFUNCTION(Server, Reliable, WithValidation)    
	void ServerFire(const FVector pos, const FVector dir);

	// Multicast so all clients run shoot effects    
	UFUNCTION(NetMultiCast, unreliable)    
	void MultiCastShootEffects();

	// Called on death for all clients for hilarious death    
	UFUNCTION(NetMultiCast, unreliable)    
	void MultiCastRagdoll();

	// Play pain on owning client when hit    
	UFUNCTION(Client, Reliable)    
	void PlayPain();

};

