
#include "MultitudCharacter.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MotionControllerComponent.h"
#include "MultitudPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMultitudCharacter

AMultitudCharacter::AMultitudCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterFP_Mesh"));
	FP_Mesh->SetOnlyOwnerSee(true);
	FP_Mesh->SetupAttachment(FirstPersonCameraComponent);
	FP_Mesh->bCastDynamicShadow = false;
	FP_Mesh->CastShadow = false;
	FP_Mesh->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	FP_Mesh->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	//------------------------------------------

	// Create a gun mesh component
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	TP_Gun->SetOwnerNoSee(true);
	//TP_Gun->AttachToComponent(FP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("hand_rSocket"));
	TP_Gun->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	GetMesh()->SetOwnerNoSee(true);

	// Create particles 
	TP_GunShotParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSysTP"));
	TP_GunShotParticle->bAutoActivate = false; 
	TP_GunShotParticle->SetupAttachment(TP_Gun);
	TP_GunShotParticle->SetOwnerNoSee(true);

	// Create particle 
	FP_GunShotParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FP_GunShotParticle"));
	FP_GunShotParticle->bAutoActivate = false; 
	FP_GunShotParticle->SetupAttachment(FP_Gun);
	FP_GunShotParticle->SetOnlyOwnerSee(true);

	BulletParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletSysTP")); 
	BulletParticle->bAutoActivate = false; 
	BulletParticle->SetupAttachment(FirstPersonCameraComponent);

	//------------------------------------------

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Create VR Controllers.
	//R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	//R_MotionController->Hand = EControllerHand::Right;
	//R_MotionController->SetupAttachment(RootComponent);
	//L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	//L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	//VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	//VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	//VR_Gun->bCastDynamicShadow = false;
	//VR_Gun->CastShadow = false;
	//VR_Gun->SetupAttachment(R_MotionController);
	//VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	//VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	//VR_MuzzleLocation->SetupAttachment(VR_Gun);
	//VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	//VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AMultitudCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (Role != ROLE_Authority) 
	{ 
		SetTeam(CurrentTeam); 
	}

	FP_Gun->AttachToComponent(FP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	/*if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		FP_Mesh->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		FP_Mesh->SetHiddenInGame(false, true);
	}*/
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultitudCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMultitudCharacter::OnFire);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMultitudCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMultitudCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMultitudCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMultitudCharacter::LookUpAtRate);
}

void AMultitudCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMultitudCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMultitudCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMultitudCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMultitudCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultitudCharacter, CurrentTeam);
}

void AMultitudCharacter::SetTeam_Implementation(ETeam NewTeam)
{
	FLinearColor outColour;

	if (NewTeam == ETeam::BLUE_TEAM)
	{
		outColour = FLinearColor(0.0f, 0.0f, 0.5f);
	}
	else
	{
		outColour = FLinearColor(0.5f, 0.0f, 0.0f);
	}

	if (DynamicMat == nullptr)
	{
		DynamicMat = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
		DynamicMat->SetVectorParameterValue(TEXT("BodyColor"), outColour);

		GetMesh()->SetMaterial(0, DynamicMat);        
		FP_Mesh->SetMaterial(0, DynamicMat);
	}
}

void AMultitudCharacter::OnFire()
{
	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FP_FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = FP_Mesh->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FP_FireAnimation, 1.f);
		}
	}

	// Play the FP particle effect if specified    
	if (FP_GunShotParticle != nullptr)    
	{        
		FP_GunShotParticle->Activate(true);    
	} 

	FVector mousePos; 
	FVector mouseDir;

	APlayerController* pController = Cast<APlayerController>(GetController());
	FVector2D ScreenPos = GEngine->GameViewport->Viewport->GetSizeXY();

	pController->DeprojectScreenPositionToWorld(ScreenPos.X / 2.0f, ScreenPos.Y / 2.0f, mousePos, mouseDir); mouseDir *= 10000000.0f;
	ServerFire(mousePos, mouseDir);
}

bool AMultitudCharacter::ServerFire_Validate(const FVector pos, const FVector dir)
{
	if (pos != FVector(ForceInit) && dir != FVector(ForceInit)) 
	{ 
		return true; 
	}
	else 
	{ 
		return false; 
	}
}

void AMultitudCharacter::ServerFire_Implementation(const FVector pos, const FVector dir) 
{ 
	Fire(pos, dir);    
	MultiCastShootEffects(); 
}

void AMultitudCharacter::MultiCastShootEffects_Implementation()
{
	// try and play a firing animation if specified
	if (TP_FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(TP_FireAnimation, 1.f);
		}
	}

	// try and play the sound if specified 
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (TP_GunShotParticle != nullptr) 
	{ 
		TP_GunShotParticle->Activate(true); 
	}

	if (BulletParticle != nullptr) 
	{ 
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletParticle->Template, BulletParticle->GetComponentLocation(), BulletParticle->GetComponentRotation()); 
	}
}

void AMultitudCharacter::Fire(const FVector pos, const FVector dir)
{
	// Perform Raycast 
	FCollisionObjectQueryParams ObjQuery;    
	ObjQuery.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	FCollisionQueryParams ColQuery;    
	ColQuery.AddIgnoredActor(this);

	FHitResult HitRes;    
	GetWorld()->LineTraceSingleByObjectType(HitRes, pos, dir, ObjQuery, ColQuery);

	DrawDebugLine(GetWorld(), pos, dir, FColor::Red, true, 100, 0, 5.0f);

	if (HitRes.bBlockingHit)
	{
		AMultitudCharacter* OtherChar = Cast <AMultitudCharacter> (HitRes.GetActor());

		if (OtherChar != nullptr && OtherChar->GetMultitudPlayerState()->Team != this->GetMultitudPlayerState()->Team)
		{
			FDamageEvent thisEvent(UDamageType::StaticClass());            
			OtherChar->TakeDamage(10.0f, thisEvent, this->GetController(), this);

			APlayerController* thisPC = Cast<APlayerController>(GetController());
			thisPC->ClientPlayForceFeedback(HitSuccessFeedback, false, NAME_None);
		}
	}
}

float AMultitudCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (Role == ROLE_Authority && DamageCauser != this && MultitudPlayerState->Health > 0)
	{
		MultitudPlayerState->Health -= Damage;        
		PlayPain();

		if (MultitudPlayerState->Health <= 0)
		{
			MultitudPlayerState->Deaths++;

			// Player has died time to respawn
			MultiCastRagdoll();
			AMultitudCharacter* OtherChar = Cast<AMultitudCharacter>(DamageCauser);

			if (OtherChar)
			{
				OtherChar->MultitudPlayerState->Score += 1.0f;
			}

			// After 3 seconds respawn            
			FTimerHandle thisTimer; 
			GetWorldTimerManager().SetTimer<AMultitudCharacter>(thisTimer, this, &AMultitudCharacter::Respawn, 3.0f, false);
		}
	}

	return Damage;
}

void AMultitudCharacter::PlayPain_Implementation()
{
	if (Role == ROLE_AutonomousProxy) 
	{ 
		UGameplayStatics::PlaySoundAtLocation(this, PainSound, GetActorLocation()); 
	}
}

void AMultitudCharacter::MultiCastRagdoll_Implementation()
{
	GetMesh()->SetPhysicsBlendWeight(1.0f);    
	GetMesh()->SetSimulatePhysics(true);    
	GetMesh()->SetCollisionProfileName("Ragdoll");
}

void AMultitudCharacter::Respawn()
{
	if (Role == ROLE_Authority) 
	{        
		// Get Location from game mode        
		MultitudPlayerState->Health = 100.0f;            
		Cast<AMultitudGameMode>(GetWorld()->GetAuthGameMode())->Respawn(this);       
		Destroy(true, true);    
	} 
}

void AMultitudCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	MultitudPlayerState = Cast<AMultitudPlayerState>(PlayerState);

	if (Role == ROLE_Authority && MultitudPlayerState != nullptr) 
	{ 
		MultitudPlayerState->Health = 100.0f; 
	}
}

AMultitudPlayerState* AMultitudCharacter::GetMultitudPlayerState() 
{ 
	if (MultitudPlayerState) 
	{ 
		return MultitudPlayerState; 
	} 
	else 
	{ 
		MultitudPlayerState = Cast<AMultitudPlayerState>(PlayerState);        
		return MultitudPlayerState; 
	} 
}

void AMultitudCharacter::SetMultitudPlayerState(AMultitudPlayerState* newPS)
{
	// Ensure PS is valid and only set on server
	if (newPS && Role == ROLE_Authority)
	{
		MultitudPlayerState = newPS;        
		PlayerState = newPS;
	}
}
