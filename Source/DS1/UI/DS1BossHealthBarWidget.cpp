// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DS1BossHealthBarWidget.h"

#include "DS1StatBarWidget.h"
#include "Components/ProgressBar.h"

UDS1BossHealthBarWidget::UDS1BossHealthBarWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UDS1BossHealthBarWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	// if (BossHpProgressBar)
	// {
	// 	BossHpProgressBar->SetFillColorAndOpacity(FillColorAndOpacity);
	// }
}

void UDS1BossHealthBarWidget::Init(UDS1AttributeComponent* AttributeComponent)
{
	if (AttributeComponent)
	{
		// 델리게이트
		AttributeComponent->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
		AttributeComponent->BroadcastAttributeChanged(EDS1AttributeType::Health);
	}
}

void UDS1BossHealthBarWidget::OnAttributeChanged(EDS1AttributeType AttributeType, float InValue)
{
	if (AttributeType == EDS1AttributeType::Health)
	{
		BossHpProgressBar->SetPercent(InValue);
	}
}


void UDS1BossHealthBarWidget::SetHpRatio(float Ratio) const
{
	BossHpProgressBar->SetPercent(Ratio);
}
