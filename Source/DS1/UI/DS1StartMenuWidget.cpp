// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DS1StartMenuWidget.h"

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

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Hello, Debug World!"));
		}
	}, 0.1f, false);
	
}

void UDS1StartMenuWidget::OptionButtonClick()
{
}

void UDS1StartMenuWidget::QuitButtonClick()
{
}
