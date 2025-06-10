// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/DS1FistWeapon.h"

#include "Animation/DS1AnimInstance.h"
#include "Characters/DS1Character.h"
#include "Components/DS1CombatComponent.h"
#include "Components/DS1WeaponCollisionComponent.h"

ADS1FistWeapon::ADS1FistWeapon()
{
}

void ADS1FistWeapon::EquipItem()
{
	CombatComponent = GetOwner()->GetComponentByClass<UDS1CombatComponent>();

	if (CombatComponent)
	{
		CombatComponent->SetWeapon(this);

		if (ADS1Character* OwnerCharacter = Cast<ADS1Character>(GetOwner()))
		{
			// 맨손을 무기로 사용하기 때문에 캐릭터의 스켈레탈메시에 바로 설정
			// 오른손/왼손에 Collision 세팅 
			RightWeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());
			LeftWeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());

			// 주먹은 바로 전투 상태
			CombatComponent->SetCombatEnabled(true);

			// 장착한 무기의 CombatType으로 업데이트.
			if (UDS1AnimInstance* Anim = Cast<UDS1AnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance()))
			{
				Anim->UpdateCombatMode(CombatType);
			}

			// 무기를 소유한 OwnerActor를 충돌에서 무시합니다.
			RightWeaponCollision->AddIgnoredActor(OwnerCharacter);
			LeftWeaponCollision->AddIgnoredActor(OwnerCharacter);
		}
	}
}
