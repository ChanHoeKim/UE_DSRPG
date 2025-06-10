// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DS1StartMenuWidget.h"

#include "DS1PlayerHUDWidget.h"
#include "Characters/DS1Character.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UDS1StartMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Start_Button)
	{
		Start_Button->OnClicked.AddDynamic(this, &ThisClass::StartButtonClick);
	}
	if (Option_Button)
	{
		Option_Button->OnClicked.AddDynamic(this, &ThisClass::OptionButtonClick);
	}
	if (Quit_Button)
	{
		Quit_Button->OnClicked.AddDynamic(this, &ThisClass::QuitButtonClick);
	}
}

void UDS1StartMenuWidget::StartButtonClick()
{
	RemoveFromParent();
	// 0.1초 뒤에 OpenLevel 실행
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		UGameplayStatics::OpenLevel(this, FName("DevMap"));
		// 입력 모드 전환
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
			// ADS1Character* Character = Cast<ADS1Character>(PC->GetPawn());
			// if (Character)
			// {
			// 	Character->SetIsViewPlayerHUD(true);
			// }
			
		}
		
	}, 0.1f, false);
	
}

void UDS1StartMenuWidget::OptionButtonClick()
{
}

void UDS1StartMenuWidget::QuitButtonClick()
{
}
