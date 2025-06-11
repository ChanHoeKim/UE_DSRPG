// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/DS1GameInstance.h"

#include "Blueprint/UserWidget.h"
#include "UI/DS1StartMenuWidget.h"
#include "Engine/World.h"
#include "UI/DS1InputBlockingWidget.h"
#include "UI/DS1OptionWidget.h"
#include "UI/DS1PlayerHUDWidget.h"

void UDS1GameInstance::Init()
{
	Super::Init();
	
	// Player HUD를 생성
	// if (PlayerHUDWidgetClass)
	// {
	// 	PlayerHUDWidget = CreateWidget<UDS1PlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
	// 	// if (PlayerHUDWidget)
	// 	// {
	// 	// 	UE_LOG(LogTemp, Log, TEXT("PlayerHUDWidgetClass 생성"));
	// 	// 	PlayerHUDWidget->AddToViewport();
	// 	// }
	// }
	
	if (OptionWidgetClass)
	{
		if (!OptionWidget)
		{
			OptionWidget = Cast<UDS1OptionWidget>(CreateWidget<UUserWidget>(GetWorld(), OptionWidgetClass));
		}
	}
	
	if (InputBlockingWidgetClass)
	{
		if (!InputBlockingWidget)
		{
			InputBlockingWidget = Cast<UDS1InputBlockingWidget>(CreateWidget<UUserWidget>(GetWorld(), InputBlockingWidgetClass));
		}
	}
}

void UDS1GameInstance::MakeAndShowStartWidget()
{
	if (!StartWidgetClass)
	{
		// 이미 생성했으면 다시 생성하지 않음
		return;
	}
	
	StartWidget = CreateWidget<UDS1StartMenuWidget>(GetWorld(), StartWidgetClass);
	if (StartWidget)
	{
		StartWidget->AddToViewport();
		
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			PC->bShowMouseCursor = true;
			// FInputModeUIOnly InputMode;
			// InputMode.SetWidgetToFocus(StartWidget->TakeWidget());
			// PC->SetInputMode(InputMode);
		}
	}
}