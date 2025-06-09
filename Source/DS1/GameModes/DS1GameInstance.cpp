// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/DS1GameInstance.h"

#include "Blueprint/UserWidget.h"
#include "UI/DS1StartMenuWidget.h"
#include "Engine/World.h"

void UDS1GameInstance::Init()
{
	Super::Init();
	// FWorldDelegates::OnPostLoadMapWithWorld.AddUObject(this, &UDS1GameInstance::OnPostLoadMap);
}

void UDS1GameInstance::ShowStartWidget()
{
	if (!StartWidgetClass)
	{
		// 이미 생성했으면 다시 생성하지 않음
		return;
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			/* Key */ -1,                    // -1: 매번 새로운 메시지, 0 이상의 값은 덮어씀
			/* Duration */ 5.0f,             // 지속 시간 (초)
			/* Color */ FColor::Green,       // 출력 색상
			/* Message */ TEXT("Hello!")     // 출력 문자열
		);
	}

	UE_LOG(LogTemp, Log, TEXT("@@@@@@@@@@@@@@@@@"));

	StartWidget = CreateWidget<UDS1StartMenuWidget>(GetWorld(), StartWidgetClass);
	if (StartWidget)
	{
		StartWidget->AddToViewport();

		
		
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			PC->bShowMouseCursor = true;
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(StartWidget->TakeWidget());
			PC->SetInputMode(InputMode);
		}
	}
}