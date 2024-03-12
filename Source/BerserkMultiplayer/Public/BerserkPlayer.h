// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/BoxComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "BerserkProjectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"


#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BerserkPlayer.generated.h"

UCLASS()
class BERSERKMULTIPLAYER_API ABerserkPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABerserkPlayer();

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	void ApplyDamage(FVector Knockback = FVector::ZeroVector, float KnockbackStrength = 0.0f);

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshGunComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	/** Projectile class to spawn */
	UPROPERTY(EditAnywhere, Category = Projectile)
	TSubclassOf<class ABerserkProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 HP = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Asset References")
	UNiagaraSystem* DeathParticlesClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
	USoundBase* DeathSound;

private:
	/** Movement component used for movement logic in various movement modes (walking, falling, etc), containing relevant settings and functions to control movement. */
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPawnMovementComponent> PawnMovement;

	FVector AimDirection = FVector::ForwardVector;

	FVector AimTotalThisTick = FVector::ZeroVector;

	FVector CurrentKnockbackDirection = FVector::ZeroVector;
	float CurrentKnockbackStrength = 0.0f;

	bool CanFire = true;
	
	bool Invincible = false;

	bool Frozen = false;

	bool GettingKnockedBack = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void AllowFiringAgain();

	void EndInvincibility();

	void EndFrozen();

	void StopKnockBack();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called for forwards/backward input */
	void AimForward(float Value);

	/** Called for side to side input */
	void AimRight(float Value);

	void Fire();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


};
