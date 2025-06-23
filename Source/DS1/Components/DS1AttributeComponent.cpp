// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DS1AttributeComponent.h"

#include "DS1GameplayTags.h"
#include "DS1StateComponent.h"
#include "Kismet/GameplayStatics.h"

UDS1AttributeComponent::UDS1AttributeComponent()
{
	//PrimaryComponentTick.bCanEverTick = true;

	HitDilationTime = 0.f;
	
}


void UDS1AttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UDS1AttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UDS1AttributeComponent::CheckHasEnoughStamina(float StaminaCost) const
{
	return BaseStamina >= StaminaCost;
}

void UDS1AttributeComponent::DecreaseStamina(float StaminaCost)
{
	BaseStamina = FMath::Clamp(BaseStamina - StaminaCost, 0.f, MaxStamina);

	BroadcastAttributeChanged(EDS1AttributeType::Stamina);
}

void UDS1AttributeComponent::ToggleRecoveryStamina(bool bEnabled, float StartDelay)
{
	if (bEnabled)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(RecoveryStaminaTimerHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(RecoveryStaminaTimerHandle, this, &ThisClass::RecoveryStaminaHandler, 0.1f, true, StartDelay);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(RecoveryStaminaTimerHandle);
	}
}

void UDS1AttributeComponent::BroadcastAttributeChanged(EDS1AttributeType InAttributeType) const
{
	if (OnAttributeChanged.IsBound())
	{
		// Type에 따라서 값을 골라줌.
		float Ratio = 0.f;
		switch (InAttributeType)
		{
		case EDS1AttributeType::Stamina:
			Ratio = GetStaminaRatio();
			break;

		case EDS1AttributeType::Health:
			Ratio = GetHealthRatio();
			break;
		}

		OnAttributeChanged.Broadcast(InAttributeType, Ratio);
	}
}

void UDS1AttributeComponent::TakeDamageAmount(float DamageAmount)
{
	// 방어력 공식 적용.
	const float MaxDamage = DamageAmount * (DamageAmount / (DamageAmount + DefenseStat));
	const float TotalDamage = FMath::Clamp(DamageAmount, 0, MaxDamage);

	//GEngine->AddOnScreenDebugMessage(6, 1.f, FColor::Red, FString::Printf(TEXT("DamageAmount:%f, TotalDamage: %f"), DamageAmount, TotalDamage));

	//카메라 흔들림
	HitImpactCameraShake();
	
	if (!GetWorld()->GetTimerManager().IsTimerActive(HitImpactTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(
		HitImpactTimerHandle,         // 타이머 핸들
		this,              // 호출할 대상 (보통 this)
		&ThisClass::HitImpactSlow,  // 호출할 함수 포인터
		GetWorld()->GetDeltaSeconds(),                // 대기 시간 (초 단위)
		true,               // 반복 여부 (true = 무한 반복)
		0.f           // (옵션) 첫 실행까지 지연 시간 (기본 = Time)
		);
	}

	//HitImpactCameraShake();
	
	// 체력 차감.
	BaseHealth = FMath::Clamp(BaseHealth - TotalDamage, 0.f, MaxHealth);

	BroadcastAttributeChanged(EDS1AttributeType::Health);

	if (BaseHealth <= 0.f)
	{
		// Call Death Delegate
		if (OnDeath.IsBound())
		{
			OnDeath.Broadcast();
		}

		// Set Death State
		if (UDS1StateComponent* StateComp = GetOwner()->FindComponentByClass<UDS1StateComponent>())
		{
			StateComp->SetState(DS1GameplayTags::Character_State_Death);
		}
	}
}

void UDS1AttributeComponent::HealPlayer(float HealAmount)
{
	BaseHealth = FMath::Clamp(BaseHealth + HealAmount, 0.f, MaxHealth);
	BroadcastAttributeChanged(EDS1AttributeType::Health);
}

void UDS1AttributeComponent::HitImpactSlow()
{
	check(GetWorld())

	HitDilationTime = FMath::FInterpTo(HitDilationTime, 1.0f, GetWorld()->GetDeltaSeconds(), 15.0f);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), HitDilationTime);
	
	
	// 거의 다 복원되면 정지
	if (FMath::IsNearlyEqual(HitDilationTime, 1.0f, 0.01f))
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		GetWorld()->GetTimerManager().ClearTimer(HitImpactTimerHandle);
		HitDilationTime = 0.f;
	}
}

void UDS1AttributeComponent::HitImpactCameraShake()
{
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && HitCameraShakeClass)
	{
		PC->ClientStartCameraShake(HitCameraShakeClass, 5.f);
		// if (GEngine)
		// {
		// 	GEngine->AddOnScreenDebugMessage(
		// 		-1,                             // Key (-1 = 항상 새 메시지로)
		// 		5.0f,                           // Duration (초)
		// 		FColor::Green,                 // 텍스트 색상
		// 		TEXT("Camera Shake")   // 출력 메시지
		// 	);
		// }
	}
}

void UDS1AttributeComponent::RecoveryStaminaHandler()
{
	BaseStamina = FMath::Clamp(BaseStamina + StaminaRegenRate, 0.f, MaxStamina);

	BroadcastAttributeChanged(EDS1AttributeType::Stamina);

	if (BaseStamina >= MaxStamina)
	{
		ToggleRecoveryStamina(false);
	}
}

