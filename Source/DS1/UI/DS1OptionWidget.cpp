// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DS1OptionWidget.h"

#include "DS1InputBlockingWidget.h"
#include "Components/Button.h"
#include "GameModes/DS1GameInstance.h"


UDS1OptionWidget::UDS1OptionWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}


void UDS1OptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Decline_Button)
	{
		Decline_Button->OnClicked.AddDynamic(this, &ThisClass::DeclineButtonClick);
	}
	
}

void UDS1OptionWidget::DeclineButtonClick()
{
	if (this->IsInViewport())
	{
		RemoveFromParent();
	}
	UDS1GameInstance* GI = Cast<UDS1GameInstance>(GetWorld()->GetGameInstance());
	if (GI && GI->GetInputBlockingWidget()->IsInViewport())
	{
		GI->GetInputBlockingWidget()->RemoveFromParent();	
	}
}