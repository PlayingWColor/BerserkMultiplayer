// Fill out your copyright notice in the Description page of Project Settings.


#include "BerserkCharacter.h"

// Sets default values
ABerserkCharacter::ABerserkCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABerserkCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABerserkCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABerserkCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

