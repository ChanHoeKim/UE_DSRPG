// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DS1EnemyHpBarWidget.h"

#include "Components/ProgressBar.h"

UDS1EnemyHpBarWidget::UDS1EnemyHpBarWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UDS1EnemyHpBarWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UDS1EnemyHpBarWidget::SetHpRatio(float Ratio) const
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Ratio);
	}
}
