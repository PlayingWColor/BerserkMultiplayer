// Fill out your copyright notice in the Description page of Project Settings.

#include "BerserkHUD.h"

#include "Components/TextBlock.h"

#include "BerserkUserWidget.h"



ABerserkHUD::ABerserkHUD()
{
	/** Load class from asset in Content Browser of the UserWidget we created (UGUserWidget) */
	static ConstructorHelpers::FClassFinder<UBerserkUserWidget> WidgetAsset(TEXT("/Game/UI/WinningUI"));
	if (WidgetAsset.Succeeded())
	{
		/** Assign the class of the loaded asset to the WigetClass variable, which is a "subclass" of UUserWidget : Which our asset class is */
		WinningWidgetClass = WidgetAsset.Class;
	}
}

void ABerserkHUD::DisplayWinningText(const FText& WinText)
{

	if (WinningWidgetClass)
	{
		UUserWidget* WinWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), WinningWidgetClass, "WinningWidget");
		if (WinWidget != nullptr)
		{
			WinWidget->AddToViewport();
			UTextBlock* TextWidget = Cast<UTextBlock>(WinWidget->GetWidgetFromName(TEXT("WinningText")));

			if (TextWidget != nullptr)
			{
				TextWidget->SetText(WinText);
			}
		}
	}

}