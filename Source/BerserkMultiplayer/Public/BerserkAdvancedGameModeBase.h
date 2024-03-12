// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BerserkPlayer.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "BerserkAdvancedGameModeBase.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class BERSERKMULTIPLAYER_API ABerserkAdvancedGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:
	ABerserkAdvancedGameModeBase();
	//Possess a player if it is available
	void PossessAvailablePlayer(APlayerController* controller);

	void UnPossessPlayer(ABerserkPlayer* PlayerToUnpossess, ABerserkPlayer* RemainingPlayer);

	void Win();

	void DoReset();

	UFUNCTION(BlueprintCallable, Category = "Initialization")
	void InitializeFromLevelBlueprint(ABerserkPlayer* BerserkPlayer1, ABerserkPlayer* BerserkPlayer2, AActor* NewOverheadCamera);

	UFUNCTION(BlueprintCallable, Category = "Initialization")
	void CriticalEventOccurred();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control|Player")
	ABerserkPlayer* Player1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control|Player")
	ABerserkPlayer* Player2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control|Player")
	AActor* OverheadCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	bool WinConditionsMet = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	float CameraShake = 0;
};
