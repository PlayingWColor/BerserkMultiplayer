// Fill out your copyright notice in the Description page of Project Settings.


#include "BerserkPlayer.h"

#include "BerserkAdvancedGameModeBase.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
ABerserkPlayer::ABerserkPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	//RootComponent->SetMobility(EComponentMobility::Movable);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	if (BoxComponent != nullptr)
	{
		BoxComponent->InitBoxExtent(FVector::OneVector * 50);
		BoxComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

		//BoxComponent->CanCharacterStepUpOn = ECB_No;
		BoxComponent->SetShouldUpdatePhysicsVolume(true);
		BoxComponent->SetCanEverAffectNavigation(false);
		BoxComponent->bDynamicObstacle = true;
		BoxComponent->CanCharacterStepUpOn = ECB_No;

		RootComponent = BoxComponent;


		StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
		if (StaticMeshComponent != nullptr)
		{
			StaticMeshComponent->bCastDynamicShadow = true;
			StaticMeshComponent->bAffectDynamicIndirectLighting = true;
			StaticMeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
			StaticMeshComponent->SetupAttachment(BoxComponent);
			//StaticMeshComponent->SetGenerateOverlapEvents(false);
			StaticMeshComponent->SetCanEverAffectNavigation(false);


			// Load cube static mesh from package
			static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeStaticMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
			if (CubeStaticMesh.Object != nullptr)
			{
				StaticMeshComponent->SetStaticMesh(CubeStaticMesh.Object);
			}
		}

		//Gun
		StaticMeshGunComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshGunComponent"));
		if (StaticMeshGunComponent != nullptr)
		{
			StaticMeshGunComponent->bCastDynamicShadow = true;
			StaticMeshGunComponent->bAffectDynamicIndirectLighting = true;
			StaticMeshGunComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
			StaticMeshGunComponent->SetupAttachment(BoxComponent);
			//StaticMeshComponent->SetGenerateOverlapEvents(false);
			StaticMeshGunComponent->SetCanEverAffectNavigation(false);

			StaticMeshGunComponent->SetWorldScale3D(FVector(0.5f,0.2f,0.5f));

			// Load cube static mesh from package
			static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeStaticMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
			if (CubeStaticMesh.Object != nullptr)
			{
				StaticMeshGunComponent->SetStaticMesh(CubeStaticMesh.Object);
			}
		}

		PawnMovement = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("MovementComponent"));
		if (PawnMovement)
		{
			PawnMovement->UpdatedComponent = BoxComponent;
		}

	}

	/** Load class from asset in Content Browser of the UserWidget we created (UGUserWidget) */
	//static ConstructorHelpers::FClassFinder<UNiagaraSystem> DeathParticlesAsset(TEXT("/Game/Niagara/DeathNS"));
	//if (DeathParticlesAsset.Succeeded())
	//{
		/** Assign the class of the loaded asset to the WigetClass variable, which is a "subclass" of UUserWidget : Which our asset class is */
	//	DeathParticlesClass = DeathParticlesAsset.Class;
	//}
}

UPawnMovementComponent* ABerserkPlayer::GetMovementComponent() const
{
	return PawnMovement;
}

// Called when the game starts or when spawned
void ABerserkPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABerserkPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Invincible && StaticMeshComponent != nullptr)
	{
		if(StaticMeshComponent->bHiddenInGame)
			StaticMeshComponent->SetHiddenInGame(false);
		else
			StaticMeshComponent->SetHiddenInGame(true);
		
	}
	else if(StaticMeshComponent != nullptr && StaticMeshComponent->bHiddenInGame)
		StaticMeshComponent->SetHiddenInGame(false);

	if (GettingKnockedBack)
	{
		AddMovementInput(CurrentKnockbackDirection, CurrentKnockbackStrength);
		return;
	}

	if (GetMovementComponent())
	{
		if (AimTotalThisTick.Length() > 0)
		{
			AimDirection = AimTotalThisTick;
			AimDirection.Normalize();
			AimTotalThisTick = FVector::ZeroVector;
		}
		
		else  if (GetMovementComponent()->GetLastInputVector().Length() > 0)
		{
			//UE_LOG(LogTemp, Log, TEXT("Length of Move Vector: %f"), (GetMovementComponent()->GetLastInputVector().Length()));
			AimDirection = GetMovementComponent()->GetLastInputVector();
			AimDirection.Normalize();
			//UE_LOG(LogTemp, Log, TEXT("Aim Vector Length: %f"), (AimDirection.Length()));
		}

		StaticMeshGunComponent->SetWorldLocationAndRotation(GetActorLocation() + AimDirection * 70.f, AimDirection.Rotation());
	}


}

// Called to bind functionality to input
void ABerserkPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveYAxis", this, &ABerserkPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveXAxis", this, &ABerserkPlayer::MoveRight);

	PlayerInputComponent->BindAxis("AimYAxis", this, &ABerserkPlayer::AimForward);
	PlayerInputComponent->BindAxis("AimXAxis", this, &ABerserkPlayer::AimRight);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABerserkPlayer::Fire);
}

void ABerserkPlayer::MoveForward(float Value)
{
	if (!Frozen && ((Controller != nullptr) && (Value != 0.0f)))
	{
		//UE_LOG(LogTemp, Log, TEXT("%s MoveForward as %s"), *GetName(), *(Controller->GetName()));
		
		//UE_LOG(LogTemp, Log, TEXT("%s"), (IsMoveInputIgnored() ? TEXT("true") : TEXT("false")));

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABerserkPlayer::MoveRight(float Value)
{
	if (!Frozen && ((Controller != nullptr) && (Value != 0.0f)))
	{
		//UE_LOG(LogTemp, Log, TEXT("%s MoveRight as %s"), *GetName(), *(Controller->GetName()));
		
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ABerserkPlayer::AimForward(float Value)
{
	if (!Frozen && ((Controller != nullptr) && (Value != 0.0f)))
	{
		//UE_LOG(LogTemp, Log, TEXT("%s MoveForward as %s"), *GetName(), *(Controller->GetName()));

		//UE_LOG(LogTemp, Log, TEXT("%s"), (IsMoveInputIgnored() ? TEXT("true") : TEXT("false")));

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AimTotalThisTick -= Direction * Value;
	}
}

void ABerserkPlayer::AimRight(float Value)
{
	if (!Frozen && ((Controller != nullptr) && (Value != 0.0f)))
	{
		//UE_LOG(LogTemp, Log, TEXT("%s MoveRight as %s"), *GetName(), *(Controller->GetName()));

		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AimTotalThisTick += Direction * Value;
	}
}

void ABerserkPlayer::Fire()
{
	if (GetController() == nullptr || !CanFire || Frozen)
	{
		return;
	}

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(GetController());
			//const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetActorLocation() + AimDirection * 100;

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at the muzzle
			World->SpawnActor<ABerserkProjectile>(ProjectileClass, SpawnLocation, AimDirection.Rotation(), ActorSpawnParams);

			// Try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			CanFire = false;
			FTimerHandle UnusedHandle;
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &ABerserkPlayer::AllowFiringAgain, 0.5f, false);
		}
	}
	/*
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}*/
}

void ABerserkPlayer::ApplyDamage(FVector KnockbackDirection, float KnockbackStrength)
{
	if (Invincible)
		return;
	
	CurrentKnockbackDirection = KnockbackDirection;
	CurrentKnockbackStrength = KnockbackStrength;

	HP -= 1;
	auto GameMode = Cast<ABerserkAdvancedGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode != nullptr)
	{
		GameMode->CriticalEventOccurred();
	}
	if (HP == 0)
	{
		//Spawn Death Particles
		if (DeathParticlesClass)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DeathParticlesClass, GetActorLocation());
		}

		// Try and play the sound if specified
		if (DeathSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
		}

		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
		CanFire = false;
	}
	else
	{
		// Try and play the sound if specified
		if (HitSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		}

		if (GameMode != nullptr)
		{
			if (GameMode->CameraShake < 0.25f)
				GameMode->CameraShake = 0.25f;
		}
		
		UPawnMovementComponent* MovementComponent = GetMovementComponent();
		if(MovementComponent)
			MovementComponent->AddInputVector(KnockbackDirection *KnockbackStrength, true);
		
		Invincible = true;
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &ABerserkPlayer::EndInvincibility, 0.67f, false);

		Frozen = true;
		FTimerHandle UnusedHandleTwo;
		GetWorldTimerManager().SetTimer(UnusedHandleTwo, this, &ABerserkPlayer::EndFrozen, 0.33f, false);

		GettingKnockedBack = true;
		FTimerHandle UnusedHandleThree;
		GetWorldTimerManager().SetTimer(UnusedHandleThree, this, &ABerserkPlayer::StopKnockBack, 0.1f, false);
	}

}

void ABerserkPlayer::AllowFiringAgain()
{
	CanFire = true;
}

void ABerserkPlayer::EndInvincibility()
{
	Invincible = false;
}

void ABerserkPlayer::EndFrozen()
{
	Frozen = false;
}

void ABerserkPlayer::StopKnockBack()
{
	GettingKnockedBack = false;
}