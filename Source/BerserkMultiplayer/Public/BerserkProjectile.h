// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BerserkProjectile.generated.h"



class UBoxComponent;
class UProjectileMovementComponent;

UCLASS(config = Game)
class BERSERKMULTIPLAYER_API ABerserkProjectile : public AActor
{
	GENERATED_BODY()
	


public:	
	// Sets default values for this actor's properties
	ABerserkProjectile();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UBoxComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
	USoundBase* BounceSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Asset References")
	UNiagaraSystem* BlastParticlesClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);



	/** Returns CollisionComp subobject **/
	UBoxComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};
