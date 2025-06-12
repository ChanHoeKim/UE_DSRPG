// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DS1Define.h"
#include "GameplayTagContainer.h"
#include "Equipments/DS1Equipment.h"
#include "DS1Weapon.generated.h"

class UDS1WeaponCollisionComponent;
class UDS1MontageActionData;
struct FGameplayTag;
class UDS1CombatComponent;
/**
 * 
 */
UCLASS()
class DS1_API ADS1Weapon : public ADS1Equipment
{
	GENERATED_BODY()

protected:
	//전투 소켓
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Socket")
	FName EquipSocketName;

	//비전투 소켓
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Socket")
	FName UnequipSocketName;

	//무기 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EWeaponType WeaponType = EWeaponType::SwordShield;

	//해당 무기 타입의 애님 몽타주 Data Asset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Animation")
	UDS1MontageActionData* MontageActionData;

// Component Section
protected:
	UPROPERTY(VisibleAnywhere)
	UDS1WeaponCollisionComponent* RightWeaponCollision;

	UPROPERTY(VisibleAnywhere)
	UDS1WeaponCollisionComponent* LeftWeaponCollision;
	
protected:
	UPROPERTY()
	UDS1CombatComponent* CombatComponent;


protected:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> StaminaCostMap;

	/* 기본 데미지 */
	UPROPERTY(EditAnywhere)
	float BaseDamage = 15.f;

	/* 무기별 데미지 승수 */
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> DamageMultiplierMap;

public:
	ADS1Weapon();

public:
	virtual void EquipItem() override;
	void Drop();

	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag, const int32 Index = 0) const;
	UAnimMontage* GetRandomMontageForTag(const FGameplayTag& Tag) const;
	UAnimMontage* GetHitReactMontage(const AActor* Attacker) const;

	float GetStaminaCost(const FGameplayTag& InTag) const;
	float GetAttackDamage() const;

	FORCEINLINE FName GetEquipSocketName() const { return EquipSocketName; };
	FORCEINLINE FName GetUnequipSocketName() const { return UnequipSocketName; };
	FORCEINLINE UDS1WeaponCollisionComponent* GetCollision() const { return RightWeaponCollision; };
	FORCEINLINE EWeaponType GetCombatType() const { return WeaponType; };

public:
	virtual void ActivateCollision(EWeaponCollisionType InCollisionType);
	virtual void DeactivateCollision(EWeaponCollisionType InCollisionType);

public:
	/* 무기의 Collision에 충돌한 Actor에게 Damage를 전달 */
	void OnHitActor(const FHitResult& Hit);
};


