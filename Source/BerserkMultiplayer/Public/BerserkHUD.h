// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BerserkHUD.generated.h"

/**
 * 
 */
UCLASS()
class BERSERKMULTIPLAYER_API ABerserkHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	ABerserkHUD();

	void DisplayWinningText(const FText& WinText);

	
	TSubclassOf<class UUserWidget> WinningWidgetClass;

};
