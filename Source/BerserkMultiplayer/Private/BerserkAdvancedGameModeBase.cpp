// Fill out your copyright notice in the Description page of Project Settings.

#include "BerserkAdvancedGameModeBase.h"

#include "EmptyPawn.h"
#include "BerserkHUD.h"



#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"



ABerserkAdvancedGameModeBase::ABerserkAdvancedGameModeBase()
{
	PlayerControllerClass = APlayerController::StaticClass();

	DefaultPawnClass = AEmptyPawn::StaticClass();

	HUDClass = ABerserkHUD::StaticClass();
}

void ABerserkAdvancedGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABerserkAdvancedGameModeBase::InitializeFromLevelBlueprint(ABerserkPlayer* BerserkPlayer1, ABerserkPlayer* BerserkPlayer2, AActor* NewOverheadCamera)
{
	Player1 = BerserkPlayer1;
	Player2 = BerserkPlayer2;
	OverheadCamera = NewOverheadCamera;

	//Set Up Overhead Camera
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController && OverheadCamera)
	{
		PlayerController->SetViewTarget(OverheadCamera);
	}
	ABerserkAdvancedGameModeBase::PossessAvailablePlayer(PlayerController);


	APlayerController* SecondPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 1);

	if(SecondPlayerController)
		ABerserkAdvancedGameModeBase::PossessAvailablePlayer(SecondPlayerController);
	else
		ABerserkAdvancedGameModeBase::PossessAvailablePlayer(UGameplayStatics::CreatePlayer(GetWorld()));
}

void ABerserkAdvancedGameModeBase::PossessAvailablePlayer(APlayerController* controller)
{

	if (!controller || !Player1 || !Player2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing References!!!"));
		if (!controller)
			UE_LOG(LogTemp, Warning, TEXT("Missing Reference to controller"));
		if (!Player1)
			UE_LOG(LogTemp, Warning, TEXT("Missing Reference to Player1"));
		if (!Player2)
			UE_LOG(LogTemp, Warning, TEXT("Missing Reference to Player2"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Setting Up Controller: %s"), *(controller->GetName()));

	//Make sure we're not already controlling a player
	controller->UnPossess();

	if (!Player1->IsPlayerControlled())
	{
		UE_LOG(LogTemp, Log, TEXT("Possessing player 1"));
		controller->Possess(Player1);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Player 1 is already possessed by %s"), *(Player1->GetController<APlayerController>()->GetName()));

		if (!Player2->IsPlayerControlled())
		{
			UE_LOG(LogTemp, Log, TEXT("Possessing player 2"));
			controller->Possess(Player2);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Player 2 is already possessed by %s"), *(Player2->GetController<APlayerController>()->GetName()));
		}
	}



	if (OverheadCamera)
		controller->SetViewTarget(OverheadCamera);

	if (controller->GetPawn())
	{
		UE_LOG(LogTemp, Log, TEXT("Possessed Player with name: %s"), *(controller->GetPawn()->GetName()));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Possess a Player!"));
	}
	
}

void ABerserkAdvancedGameModeBase::UnPossessPlayer(ABerserkPlayer* PlayerToUnpossess, ABerserkPlayer* RemainingPlayer)
{
	if (PlayerToUnpossess == nullptr || RemainingPlayer == nullptr)
		return;

	APlayerController* controller = Cast<APlayerController>(PlayerToUnpossess->GetController());
	if (controller)
	{
		controller->UnPossess();
	}

	APlayerController* otherController = Cast<APlayerController>(RemainingPlayer->GetController());
	if (otherController)
	{
		if (OverheadCamera)
			otherController->SetViewTarget(OverheadCamera);
	}
}

void ABerserkAdvancedGameModeBase::CriticalEventOccurred()
{

	if (WinConditionsMet)
		return;
	
	APlayerController* FirstPlayerController = GetWorld()->GetFirstPlayerController();
	if (FirstPlayerController != nullptr)
	{
		ABerserkHUD* CurrentHUD = Cast<ABerserkHUD>(FirstPlayerController->GetHUD());
		if (CurrentHUD != nullptr)
		{
			if (Player1->HP == 0)
			{
				CurrentHUD->DisplayWinningText(FText::FromString(TEXT("Player 2 Wins!")));
				//UnPossessPlayer(Player1, Player2);
				Win();
				return;
			}

			if (Player2->HP == 0)
			{
				CurrentHUD->DisplayWinningText(FText::FromString(TEXT("Player 1 Wins!")));
				//UnPossessPlayer(Player2, Player1);
				Win();
				return;
			}
		}
	}

}

void ABerserkAdvancedGameModeBase::Win()
{
	WinConditionsMet = true;
	CameraShake = 1;
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &ABerserkAdvancedGameModeBase::DoReset, 5.0f, false);
}

void ABerserkAdvancedGameModeBase::DoReset()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}


