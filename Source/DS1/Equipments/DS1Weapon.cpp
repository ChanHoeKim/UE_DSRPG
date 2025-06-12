// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/DS1Weapon.h"

#include "DS1GameplayTags.h"
#include "DS1Shield.h"
#include "Animation/DS1AnimInstance.h"
#include "Characters/DS1Character.h"
#include "Components/DS1CombatComponent.h"
#include "Components/DS1WeaponCollisionComponent.h"
#include "Data/DS1MontageActionData.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ADS1Weapon::ADS1Weapon()
{
	/* 무기에 Collision Trace를 생성(충돌 감지)하기 위한 컴포넌트 생성 */
	RightWeaponCollision = CreateDefaultSubobject<UDS1WeaponCollisionComponent>("RightWeaponCollision");
	RightWeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);
	LeftWeaponCollision = CreateDefaultSubobject<UDS1WeaponCollisionComponent>("LeftWeaponCollision");
	LeftWeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);

	/* 공격 타입에 따른 스테미너 차별화 */
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Light, 7.f);
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Running, 12.f);
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Special, 15.f);
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Heavy, 20.f);

	/* 공격 타입에 따른 데미지 차별화(곱셈) */
	DamageMultiplierMap.Add(DS1GameplayTags::Character_Attack_Heavy, 1.8f);
	DamageMultiplierMap.Add(DS1GameplayTags::Character_Attack_Running, 1.8f);
	DamageMultiplierMap.Add(DS1GameplayTags::Character_Attack_Special, 2.1f);
}

void ADS1Weapon::EquipItem()
{
	//Super::EquipItem();
	
	CombatComponent = GetOwner()->GetComponentByClass<UDS1CombatComponent>();
	if (CombatComponent)
	{
		// Character에서 관리 가능하도록 CombatComponent에 해당 무기 등록
		CombatComponent->SetWeapon(this);

		// 캐릭터 Skeleton에서 전투 타입에 맞는 소켓에 무기 장착
		const FName AttachSocket = CombatComponent->IsCombatEnabled() ? EquipSocketName : UnequipSocketName;
		AttachToOwner(AttachSocket);

		// 무기의 콜리전 컴포넌트에 해당 무기의 Mesh를 넘김
		RightWeaponCollision->SetWeaponMesh(Mesh);

		// 장착한 무기의 CombatType으로 업데이트.
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			if (UDS1AnimInstance* Anim = Cast<UDS1AnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance()))
			{
				Anim->UpdateCombatMode(WeaponType);
			}
		}

		
		// 무기를 소유한 OwnerActor를 충돌에서 무시합니다.
		RightWeaponCollision->AddIgnoredActor(GetOwner());

		// 방패를 이미 가지고 있는지 체크해서 소켓의 위치를 잡아준다.
		if (ADS1Shield* Shield = CombatComponent->GetShield())
		{
			FName ShieldAttachSocket = Shield->GetUnequipSocketName();

			if (WeaponType == EWeaponType::SwordShield)
			{
				if (CombatComponent->IsCombatEnabled())
				{
					ShieldAttachSocket = Shield->GetEquipSocketName();
				}
			}

			Shield->AttachToOwner(ShieldAttachSocket);
		}
	}
}

void ADS1Weapon::Drop()
{
	// 맨주먹이 아니면
	if (WeaponType != EWeaponType::MeleeFists)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetSimulatePhysics(true);
	}
}

//
UAnimMontage* ADS1Weapon::GetMontageForTag(const FGameplayTag& Tag, const int32 Index) const
{
	return MontageActionData->GetMontageForTag(Tag, Index);
}

UAnimMontage* ADS1Weapon::GetRandomMontageForTag(const FGameplayTag& Tag) const
{
	return MontageActionData->GetRandomMontageForTag(Tag);
}

UAnimMontage* ADS1Weapon::GetHitReactMontage(const AActor* Attacker) const
{
	// Actor 위치에서 가해자 위치를 정면으로 보게하는 회전값
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), Attacker->GetActorLocation());

	// 현재 Actor의 회전값과 LookAt 회전값의 차이를 구함.
	// 회전 차이를 -180° ~ +180°로 정규화
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetOwner()->GetActorRotation(), LookAtRotation);
		
	// Z축 회전값 할당
	const float DeltaZ = DeltaRotation.Yaw;

	// HitDirection 초기화
	EHitDirection HitDirection = EHitDirection::Front;
	
	if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -45.f, 45.f))
	{
		HitDirection = EHitDirection::Front;
		//UE_LOG(LogTemp, Log, TEXT("Front"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 45.f, 135.f))
	{
		HitDirection = EHitDirection::Left;
		//UE_LOG(LogTemp, Log, TEXT("Left"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 135.f, 180.f)
		|| UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -180.f, -135.f))
	{
		HitDirection = EHitDirection::Back;
		//UE_LOG(LogTemp, Log, TEXT("Back"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -135.f, -45.f))
	{
		HitDirection = EHitDirection::Right;
		//UE_LOG(LogTemp, Log, TEXT("Right"));
	}

	//히트 방향에 맞는 Montage를 재생
	//중요 : 반드시 Data Asset의 인덱스에 맞게 아래 case 안에있는 함수의 인덱스를 지정해야함
	UAnimMontage* SelectedMontage = nullptr;
	switch (HitDirection)
	{
		case EHitDirection::Front:
			SelectedMontage = GetMontageForTag(DS1GameplayTags::Character_Action_HitReaction, 0);
			break;
		case EHitDirection::Back:
			SelectedMontage = GetMontageForTag(DS1GameplayTags::Character_Action_HitReaction, 1);
			break;
		case EHitDirection::Left:
			SelectedMontage = GetMontageForTag(DS1GameplayTags::Character_Action_HitReaction, 2);
			break;
		case EHitDirection::Right:
			SelectedMontage = GetMontageForTag(DS1GameplayTags::Character_Action_HitReaction, 3);
			break;
	}

	return SelectedMontage;
}

float ADS1Weapon::GetStaminaCost(const FGameplayTag& InTag) const
{
	if (StaminaCostMap.Contains(InTag))
	{
		return StaminaCostMap[InTag];
	}
	return 0.f;
}

float ADS1Weapon::GetAttackDamage() const
{
	if (const AActor* OwnerActor = GetOwner())
	{
		const FGameplayTag LastAttackType = CombatComponent->GetLastAttackType();
		if (DamageMultiplierMap.Contains(LastAttackType))
		{
			const float Multiplier = DamageMultiplierMap[LastAttackType];
			return BaseDamage * Multiplier;
		}
	}

	return BaseDamage;
}

// 무기 Collision에 Actor가 충돌되었을 때 호출될 함수
void ADS1Weapon::OnHitActor(const FHitResult& Hit)
{
	// 무기 Collision에 충돌한 Actor
	AActor* TargetActor = Hit.GetActor();
	
	// 데미지 방향(무기(this)를 소유한 Actor의 정면 방향)
	FVector DamageDirection = GetOwner()->GetActorForwardVector();

	// 데미지
	float AttackDamage = GetAttackDamage();

	UGameplayStatics::ApplyPointDamage(
		TargetActor,
		AttackDamage,
		DamageDirection,
		Hit,
		GetOwner()->GetInstigatorController(),
		this,
		nullptr);
}

//Collision 활성화
void ADS1Weapon::ActivateCollision(EWeaponCollisionType InCollisionType)
{
	switch (InCollisionType)
	{
	case EWeaponCollisionType::RightWeaponCollision:
		RightWeaponCollision->TurnOnCollision();
		break;
	case EWeaponCollisionType::LeftWeaponCollision:
		LeftWeaponCollision->TurnOnCollision();
		break;
	}
}

void ADS1Weapon::DeactivateCollision(EWeaponCollisionType InCollisionType)
{
	switch (InCollisionType)
	{
	case EWeaponCollisionType::RightWeaponCollision:
		RightWeaponCollision->TurnOffCollision();
		break;
	case EWeaponCollisionType::LeftWeaponCollision:
		LeftWeaponCollision->TurnOffCollision();
		break;
	}
}
