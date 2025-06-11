// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DS1EscWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"


void UDS1EscWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Resume_Button)
	{
		if (!Resume_Button->OnClicked.IsAlreadyBound(this, &ThisClass::ResumeButtonClick))
		{
			Resume_Button->OnClicked.AddDynamic(this, &ThisClass::ResumeButtonClick);
		}
	}
	// if (Option_Button)
	// {
	// 	if (!Option_Button->OnClicked.IsAlreadyBound(this, &ThisClass::OptionButtonClick))
	// 	{
	// 		Option_Button->OnClicked.AddDynamic(this, &ThisClass::OptionButtonClick);
	// 	}
	// }
	if (Quit_Button)
	{
		if (!Quit_Button->OnClicked.IsAlreadyBound(this,  &ThisClass::QuitButtonClick))
		{
			Quit_Button->OnClicked.AddDynamic(this, &ThisClass::QuitButtonClick);
		}
	}
}

void UDS1EscWidget::ResumeButtonClick()
{
	if (this->IsInViewport())
	{
		RemoveFromParent();
	}
	
	UGameplayStatics::SetGamePaused(GetWorld(), false);  // 게임 일시 정지
	
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->bShowMouseCursor = false;

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}


void UDS1EscWidget::QuitButtonClick()
{
	UKismetSystemLibrary::QuitGame(GetWorld(),GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
