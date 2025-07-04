// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Interfaces/DS1CombatInterface.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "DS1Character.generated.h"


class UDS1EscWidget;
class UDS1InputBlockingWidget;
class UDS1GameInstance;
class UDS1PotionInventoryComponent;
class ADS1FistWeapon;
class UDS1TargetingComponent;
class UDS1CombatComponent;
class UDS1StateComponent;
class UDS1PlayerHUDWidget;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UDS1AttributeComponent;
//class FNiagaraFunctionLibrary;

UCLASS()
class DS1_API ADS1Character : public ACharacter, public IDS1CombatInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	
	
// Input Section
private:
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SprintRollingAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

	/* 전투 활성화/비활성화 토글 */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ToggleCombatAction;

	/* Attack */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction;

	/* Heavy Attack */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* HeavyAttackAction;

	/* LockedOn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LockOnTargetAction;

	/* 왼쪽으로 타겟 전환 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftTargetAction;

	/* 오른쪽으로 타겟 전환 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightTargetAction;

	/* 방어 자세 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BlockAction;

	/* 패링 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ParryAction;

	/* 포션마시기 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ConsumeAction;

	/* ESC */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EscAction;

	/* (죽음 상태일 때만 작동하는) Enter */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EnterAction;
private:
	/* 캐릭터의 각종 스탯 관리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDS1AttributeComponent* AttributeComponent;

	/* 캐릭터의 상태 관리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDS1StateComponent* StateComponent;

	/* 무기, 전투 관리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDS1CombatComponent* CombatComponent;

	/* LockedOn Targeting */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDS1TargetingComponent* TargetingComponent;

	/* 포션 인벤토리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDS1PotionInventoryComponent* PotionInventoryComponent;

// Body parts Mesh
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* TorsoMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* LegsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* FeetMesh;


// UI
protected:
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	UDS1PlayerHUDWidget* PlayerHUDWidget;

// 주먹 무기
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ADS1FistWeapon> FistWeaponClass;

// (사운트, 타격) 효과
protected:
	UPROPERTY(EditAnywhere, Category="Effect")
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundCue* BlockingSound;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystem* BlockingParticle;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraSystem* DrinkPotionEffect;
	
protected:
	// 질주 걸음 속도
	UPROPERTY(EditAnywhere, Category="Movement Speed")
	float SprintingWalkSpeed = 750.f;

	// 일반 걸음 속도
	UPROPERTY(EditAnywhere, Category = "Movement Speed")
	float NormalWalkSpeed = 500.f;

	// 방어자세 걸음 속도 
	UPROPERTY(EditAnywhere, Category = "Movement Speed")
	float BlockingWalkSpeed = 250.f;

	// 달리기 중인지 체크
	UPROPERTY(VisibleAnywhere, Category = "Movement Speed")
	bool bSprinting = false;

	
// Combo Section
protected:
	// 콤보 시퀀스 진행중
	// bool bComboSequenceRunning = false;

	// 콤보 입력 가능 체크
	bool bCanComboInput = false;

	// 현재 콤보 카운트
	int32 ComboCounter = 0;

	// 콤보 입력 여부 
	//bool bSavedComboInput = false;

	// 콤보 리셋 타이머 핸들
	FTimerHandle ComboResetTimerHandle;

protected:
	/* 적과 대치중인 방향인지? */
	bool bFacingEnemy = false;

	/* 무적프레임 활성화 여부 */
	bool bEnabledInvincibilityFrames = false;

// Montage Section
protected:
	UPROPERTY(EditAnywhere, Category="Montage")
	UAnimMontage* RollingMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	UAnimMontage* DrinkingMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	UAnimMontage* NockDownMontage;
	
public:
	ADS1Character();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FORCEINLINE UDS1StateComponent* GetStateComponent() const { return StateComponent; };
	bool IsDeath() const;

	void SetBodyPartActive(const EDS1ArmourType ArmourType, const bool bActive) const;

	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void ImpactEffect(const FVector& Location);
	void ShieldBlockingEffect(const FVector& Location) const;
	void HitReaction(const AActor* Attacker, const EDS1DamageType InDamageType);
	void OnDeath();

	void NockDown(const AActor* InInstigator);
	
protected:
	/* 캐릭터가 이동중인지 체크 */
	bool IsMoving() const;
	bool CanToggleCombat() const;
	FORCEINLINE bool IsSprinting() const { return bSprinting; }
	//무적 프레임이 활성화 되어 있으면, 데미지를 받을 수 없음
	FORCEINLINE bool IsInvincible() const { return bEnabledInvincibilityFrames; }

	/* 이동 */
	void Move(const FInputActionValue& Values);
	
	/* 카메라 회전 */
	void Look(const FInputActionValue& Values);
	
	/* 질주 */
	void Sprinting();
	
	/* 질주 중단 */
	void StopSprint();
	
	/* 구르기 */
	void Rolling();
	
	/* 상호작용(무기 줍기) */
	void Interact();
	
	/* 전투상태 전환 */
	void ToggleCombat();
	void AutoToggleCombat();
	
	/* 공격 */
	void Attack();
	void SpecialAttack();
	void HeavyAttack();
	
	/* 락온 */
	void LockOnTarget();
	void LeftTarget();
	void RightTarget();
	
	/* 방어 */
	void Blocking();
	void BlockingEnd();
	
	/* 패링 */
	void Parrying();
	
	/* 포션 마시기 */
	void Consume();

	void DeadStatePressedEnter();
	
	void Esc();
	UPROPERTY()
	UDS1GameInstance* GI;
	UPROPERTY()
	UDS1InputBlockingWidget* InputBlockingWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Esc Widget")
	TSubclassOf<UUserWidget> EscWidgetClass;
	UPROPERTY()
	UDS1EscWidget* EscWidget; 
	
	
protected:
	/* 첫 공격 타입 반환 */
	FGameplayTag GetAttackPerform() const;

	/* 공격 가능 조건 체크 */
	bool CanPerformAttack(const FGameplayTag& AttackTypeTag) const;
	/* 공격 실행 */
	void DoAttack(const FGameplayTag& AttackTypeTag);
	/* 콤보 실행 */
	void ExecuteComboAttack(const FGameplayTag& AttackTypeTag);
	/* 콤보 초기화 */
	void ResetCombo();

	/* 방어 자세 가능 여부 */
	bool CanPlayerBlockStance() const;

	/* 방패 막기 가능 여부 */
	bool CanPerformAttackBlocking() const;

	/* 패링 가능 여부 */
	bool CanPerformParry() const;

	/* 패링 성공 여부 */
	bool ParriedAttackSucceed() const;

	/* 포션 소비(체력 회복) 가능 여부 */
	bool CanDrinkPotion()const;

	/* 포션 마실 때 공격 받음 = 포션 소비, 모션 취소 */
	void IsAttackedWhileDrinkingPotion() const;

// 콤보 AnimNotify 관련
public:
	void EnableComboWindow();
	void DisableComboWindow();
	void AttackFinished(const float ComboResetDelay);

	
public:
	virtual void ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void ToggleInvincibilityFrames(const bool bEnabled) override;

};
