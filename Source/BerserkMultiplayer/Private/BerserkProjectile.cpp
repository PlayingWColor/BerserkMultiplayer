// Fill out your copyright notice in the Description page of Project Settings.


#include "BerserkProjectile.h"
#include "BerserkPlayer.h"

#include "BerserkAdvancedGameModeBase.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ABerserkProjectile::ABerserkProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	if (CollisionComp != nullptr)
	{
		CollisionComp->InitBoxExtent(FVector::OneVector * 10);
		CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
		CollisionComp->OnComponentHit.AddDynamic(this, &ABerserkProjectile::OnHit);		// set up a notification for when this component hits something blocking

		// Players can't walk on it
		CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
		CollisionComp->CanCharacterStepUpOn = ECB_No;

		// Set as root component
		RootComponent = CollisionComp;

		StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
		if (StaticMeshComponent != nullptr)
		{
			StaticMeshComponent->bCastDynamicShadow = true;
			StaticMeshComponent->bAffectDynamicIndirectLighting = true;
			StaticMeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
			StaticMeshComponent->SetupAttachment(CollisionComp);
			//StaticMeshComponent->SetGenerateOverlapEvents(false);
			StaticMeshComponent->SetCanEverAffectNavigation(false);
			StaticMeshComponent->SetWorldScale3D(FVector::OneVector * 0.2f);

			// Load cube static mesh from package
			static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeStaticMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
			if (CubeStaticMesh.Object != nullptr)
			{
				StaticMeshComponent->SetStaticMesh(CubeStaticMesh.Object);
			}
		}

		// Use a ProjectileMovementComponent to govern this projectile's movement
		ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
		if (ProjectileMovement != nullptr)
		{
			ProjectileMovement->UpdatedComponent = CollisionComp;
			ProjectileMovement->InitialSpeed = 3000.f;
			ProjectileMovement->MaxSpeed = 3000.f;
			ProjectileMovement->bRotationFollowsVelocity = true;
			ProjectileMovement->bShouldBounce = true;
			ProjectileMovement->ProjectileGravityScale = 0;
		}
		// Die after 3 seconds by default
		InitialLifeSpan = 1.0f;
	}
}

// Called when the game starts or when spawned
void ABerserkProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (BlastParticlesClass)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BlastParticlesClass, GetActorLocation());
	}
}

// Called every frame
void ABerserkProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABerserkProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Log, TEXT("HIT"));
	
	ABerserkPlayer* player = Cast<ABerserkPlayer>(OtherActor);

	if (BlastParticlesClass)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BlastParticlesClass, GetActorLocation());
	}

	ABerserkAdvancedGameModeBase* GameMode = Cast<ABerserkAdvancedGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode != nullptr)
	{
		if (GameMode->CameraShake < 0.1f)
			GameMode->CameraShake = 0.1f;
	}

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && (player != nullptr))// && OtherComp->IsSimulatingPhysics())
	{

		
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		//OtherActor->Destroy();
		FVector NormalizedVelocity = GetVelocity();
		NormalizedVelocity.Normalize();

		player->ApplyDamage(NormalizedVelocity, 1000.0f);

		Destroy();
	}
	else
	{
		// Try and play the sound if specified
		if (BounceSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation());
		}
	}
}