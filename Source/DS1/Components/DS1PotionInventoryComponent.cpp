// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DS1PotionInventoryComponent.h"

#include "DS1AttributeComponent.h"
#include "GameFramework/Character.h"
#include "Items/DS1Potion.h"

UDS1PotionInventoryComponent::UDS1PotionInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDS1PotionInventoryComponent::DrinkPotion()
{
	// 포션의 갯수가 0 이하면 함수 바로 종료
	if (PotionQuantity <= 0)
	{
		return;
	}

	//해당 Component를 들고 있는 Owner의 Component들 중 UDS1AttributeComponent를 찾음
	if (UDS1AttributeComponent* Attribute = GetOwner()->GetComponentByClass<UDS1AttributeComponent>())
	{
		//포션 갯수 소모
		PotionQuantity--;
		//체력 회복
		Attribute->HealPlayer(PotionHealAmount);
		//HUD에서 포션 갯수 소모 처리
		BroadcastPotionUpdate();
	}
}

// 손에 포션 부착
void UDS1PotionInventoryComponent::SpawnPotion()
{
	// 손에 포션 부착하기 위해 우선 Actor 스폰
	PotionActor = GetWorld()->SpawnActor<ADS1Potion>(PotionClass, GetOwner()->GetActorTransform());
	if (PotionActor)
	{
		if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			PotionActor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), PotionSocketName);
		}
	}
}

// 손에 부착된 포션 제거
void UDS1PotionInventoryComponent::DespawnPotion()
{
	// PotionActor가 nullptr이 아니고, 파괴 되지 않았으면 실행
	if (IsValid(PotionActor))
	{
		PotionActor->Destroy();
	}
}

// 포션 갯수 HUD에 표출 
void UDS1PotionInventoryComponent::SetPointQuantity(int32 InQuantity)
{
	PotionQuantity = InQuantity;
	BroadcastPotionUpdate();
}

// 포션 갯수 관련 Delegate 실행
void UDS1PotionInventoryComponent::BroadcastPotionUpdate() const
{
	// OnUpdatePotionAmount 델리게이트에 바인딩이 한 번이라도 되었는지 확인
	if (OnUpdatePotionAmount.IsBound())
	{
		OnUpdatePotionAmount.Broadcast(PotionQuantity);
	}
}

