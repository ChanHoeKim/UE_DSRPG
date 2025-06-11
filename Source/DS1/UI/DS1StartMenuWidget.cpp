// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DS1StartMenuWidget.h"

#include "DS1InputBlockingWidget.h"
#include "DS1OptionWidget.h"
#include "DS1PlayerHUDWidget.h"
#include "Characters/DS1Character.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "GameModes/DS1GameInstance.h"
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
	//RemoveFromParent();
	
	GI = Cast<UDS1GameInstance>(GetWorld()->GetGameInstance());

	check(GI)
	if (GI)
	{
		InputBlockingWidget = GI->GetInputBlockingWidget();
		if (InputBlockingWidget)
		{
			InputBlockingWidget->AddToViewport(998);
		}
		
		
		OptionWidget = GI->GetOptionWidget();
		if (OptionWidget && !OptionWidget->IsInViewport())
		{
			OptionWidget->AddToViewport(999);
			check(OptionWidget);
			//OptionWidget->SetIsFocusable(true); 
			
			// UI 모드로 전환
			if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
			{
				PC->bShowMouseCursor = true;
				// FInputModeUIOnly InputMode;
				// InputMode.SetWidgetToFocus(OptionWidget->TakeWidget());
				// PC->SetInputMode(InputMode);

				// FInputModeUIOnly InputMode;
				// InputMode.SetWidgetToFocus(OptionWidget->TakeWidget()); // 포커스를 줄 위젯
				// InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스 잠금 해제
				// PC->SetInputMode(InputMode);
			}
		}
	}
}

void UDS1StartMenuWidget::QuitButtonClick()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
