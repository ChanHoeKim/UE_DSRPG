// Fill out your copyright notice in the Description page of Project Settings.


#include "DS1Enemy.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "DS1GameplayTags.h"
#include "DS1Define.h"
#include "GameplayTagContainer.h"
#include "AI/DS1EnemyAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/DS1AttributeComponent.h"
#include "Components/DS1CombatComponent.h"
#include "Components/DS1RotationComponent.h"
#include "Components/DS1StateComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "Equipments/DS1Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AISense_Damage.h"
#include "Sound/SoundCue.h"
#include "UI/DS1BossHealthBarWidget.h"
#include "UI/DS1EnemyHpBarWidget.h"
#include "UI/DS1StatBarWidget.h"


ADS1Enemy::ADS1Enemy()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Targeting 구체 생성및 Collision 설정
	TargetingSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("TargetingSphere"));
	TargetingSphereComponent->SetupAttachment(GetRootComponent());
	TargetingSphereComponent->SetCollisionObjectType(COLLISION_OBJECT_TARGETING);
	TargetingSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TargetingSphereComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// LockOn 위젯
	LockOnWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidgetComponent"));
	LockOnWidgetComponent->SetupAttachment(GetRootComponent());
	LockOnWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	LockOnWidgetComponent->SetDrawSize(FVector2D(30.f, 30.f));
	LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidgetComponent->SetVisibility(false);

	// HealthBar
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	HealthBarWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	HealthBarWidgetComponent->SetDrawSize(FVector2D(100.f, 5.f));
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetVisibility(false);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	AttributeComponent = CreateDefaultSubobject<UDS1AttributeComponent>(TEXT("Attribute"));
	StateComponent = CreateDefaultSubobject<UDS1StateComponent>(TEXT("State"));
	CombatComponent = CreateDefaultSubobject<UDS1CombatComponent>(TEXT("Combat"));
	RotationComponent = CreateDefaultSubobject<UDS1RotationComponent>("RotationComponent");

	// OnDeath Delegate에 함수 바인딩.
	AttributeComponent->OnDeath.AddUObject(this, &ThisClass::OnDeath);
	AttributeComponent->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
}

void ADS1Enemy::BeginPlay()
{
	Super::BeginPlay();

	// 무기 장착.
	if (DefaultWeaponClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;

		ADS1Weapon* Weapon = GetWorld()->SpawnActor<ADS1Weapon>(DefaultWeaponClass, GetActorTransform(), Params);
		CombatComponent->SetCombatEnabled(true);
		Weapon->EquipItem();
	}

	// 체력바 설정
	SetupHealthBar();
}

void ADS1Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(
	// 		/* Key */ -1,
	// 		/* Duration */ 5.0f,
	// 		/* Color */ FColor::Green,
	// 		/* Message */ FString::Printf(TEXT("%f"), AttributeComponent->GetBaseStamina())
	// 	);
	// }
}

float ADS1Enemy::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float  ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (AttributeComponent)
	{
		AttributeComponent->TakeDamageAmount(ActualDamage);
		//GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Cyan, FString::Printf(TEXT("Damaged : %f"), ActualDamage));
	}

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);

		// 데미지 방향
		FVector ShotDirection = PointDamageEvent->ShotDirection;
		// 히트 위치 (표면 접촉 관점)
		FVector ImpactPoint = PointDamageEvent->HitInfo.ImpactPoint;
		// 히트 방향
		FVector ImpactDirection = PointDamageEvent->HitInfo.ImpactNormal;
		// 히트한 객체의 Location (객체 중심 관점)
		FVector HitLocation = PointDamageEvent->HitInfo.Location;

		// AI가 데미지를 인식할 수 있도록 알려줌.
		UAISense_Damage::ReportDamageEvent(GetWorld(), this, EventInstigator->GetPawn(), ActualDamage, HitLocation, HitLocation);

		ImpactEffect(ImpactPoint);

		HitReaction(EventInstigator->GetPawn());
	}

	return ActualDamage;
}

void ADS1Enemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// World에서 제거될 때 호출
	// 혹시나 활성화 되었을 Timer 종료 
	GetWorld()->GetTimerManager().ClearTimer(ParriedDelayTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(StunnedDelayTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void ADS1Enemy::OnDeath()
{
	// Stop BehaviorTree
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->GetBrainComponent()->StopLogic(TEXT("Death"));
	}

	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Ragdoll
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionProfileName("Ragdoll");
		MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		MeshComp->SetSimulatePhysics(true);
	}

	// 무기를 떨굼.
	if (ADS1Weapon* MainWeapon = CombatComponent->GetMainWeapon())
	{
		MainWeapon->Drop();
	}

	SetDeathState();
}

void ADS1Enemy::SetDeathState()
{
	if (StateComponent)
	{
		StateComponent->SetState(DS1GameplayTags::Character_State_Death);
	}

	if (ADS1EnemyAIController* AIController = Cast<ADS1EnemyAIController>(GetController()))
	{
		AIController->StopUpdateTarget();
	}

	ToggleHealthBarVisibility(false);
}

void ADS1Enemy::OnAttributeChanged(EDS1AttributeType AttributeType, float InValue)
{
	if (AttributeType == EDS1AttributeType::Health)
	{
		if (HealthBarWidgetComponent)
		{
			if (const UDS1EnemyHpBarWidget* StatBar = Cast<UDS1EnemyHpBarWidget>(HealthBarWidgetComponent->GetWidget()))
			{
				StatBar->SetHpRatio(InValue);
			}
		}
	}
}

void ADS1Enemy::SetupHealthBar()
{
	// if (HealthBarWidgetComponent)
	// {
	// 	if (UDS1EnemyHpBarWidget* HealthBar = Cast<UDS1EnemyHpBarWidget>(HealthBarWidgetComponent->GetWidget()))
	// 	{
	// 		// 컬러 설정.
	// 		HealthBar->Fill = FLinearColor::Red;
	// 	}
	// }

	//체력 초기값 설정.
	if (AttributeComponent)
	{
		AttributeComponent->BroadcastAttributeChanged(EDS1AttributeType::Health);
	}
}

void ADS1Enemy::ImpactEffect(const FVector& Location)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Location);
	}

	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, Location);
	}
}

void ADS1Enemy::HitReaction(const AActor* Attacker)
{
	check(CombatComponent);

	//스턴
	float StunnedDelay = 0.f;
	if (StunnedRate >= FMath::RandRange(1, 100))
	{
		StateComponent->SetState(DS1GameplayTags::Character_State_Stunned);
		StunnedDelay = FMath::FRandRange(0.5f, 3.f);
	}

	if (UAnimMontage* HitReactAnimMontage = CombatComponent->GetMainWeapon()->GetHitReactMontage(Attacker))
	{
		const float DelaySeconds = PlayAnimMontage(HitReactAnimMontage) + StunnedDelay;

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]()
			{
				FGameplayTagContainer CheckTags;
				CheckTags.AddTag(DS1GameplayTags::Character_State_Stunned);
				if (StateComponent->IsCurrentStateEqualToAny(CheckTags))
				{
					StateComponent->ClearState();
				}
			});
		GetWorld()->GetTimerManager().SetTimer(StunnedDelayTimerHandle, TimerDelegate, DelaySeconds, false);
	}
}

void ADS1Enemy::OnTargeted(bool bTargeted)
{
	if (LockOnWidgetComponent)
	{
		LockOnWidgetComponent->SetVisibility(bTargeted);
	}
}

bool ADS1Enemy::CanBeTargeted()
{
	if (!StateComponent)
	{
		return false;
	}

	FGameplayTagContainer TagCheck;
	TagCheck.AddTag(DS1GameplayTags::Character_State_Death);
	return StateComponent->IsCurrentStateEqualToAny(TagCheck) == false;
}

void ADS1Enemy::ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType)
{
	if (CombatComponent)
	{
		CombatComponent->GetMainWeapon()->ActivateCollision(WeaponCollisionType);
	}
}

void ADS1Enemy::DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType)
{
	if (CombatComponent)
	{
		CombatComponent->GetMainWeapon()->DeactivateCollision(WeaponCollisionType);
	}
}

void ADS1Enemy::PerformAttack(FGameplayTag& AttackTypeTag, FOnMontageEnded& MontageEndedDelegate)
{
	check(StateComponent);
	check(AttributeComponent);
	check(CombatComponent);

	// 스턴 상태인지 체크 = 스턴 상태면 공격 못 함
	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(DS1GameplayTags::Character_State_Stunned);
	if (StateComponent->IsCurrentStateEqualToAny(CheckTags))
	{
		return;
	}

	// 무기를 갖고 있으면
	if (const ADS1Weapon* Weapon = CombatComponent->GetMainWeapon())
	{
		// 상태를 공격 중으로 변경
		StateComponent->SetState(DS1GameplayTags::Character_State_Attacking);

		// 현재 어떤 Type의 공격을 실행 했는지 CombatComponent에 넘겨줌
		// Light, Heavy, Air 등..
		CombatComponent->SetLastAttackType(AttackTypeTag);

		// 스테미너 회복 중지
		AttributeComponent->ToggleRecoveryStamina(false);

		// 공격 타입 범위에 맞는(Light, Heavy, Air 중...) 하나를 실행 
		if (UAnimMontage* Montage = Weapon->GetRandomMontageForTag(AttackTypeTag))
		{
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(Montage);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
			}
		}

		const float StaminaCost = Weapon->GetStaminaCost(AttackTypeTag);
		AttributeComponent->DecreaseStamina(StaminaCost);
		AttributeComponent->ToggleRecoveryStamina(true, 1.5f);
	}
}

void ADS1Enemy::Parried()
{
	check(StateComponent);
	check(CombatComponent);
	
	StopAnimMontage();
	StateComponent->SetState(DS1GameplayTags::Character_State_Parried);

	if (const ADS1Weapon* MainWeapon = CombatComponent->GetMainWeapon())
	{
		UAnimMontage* ParriedAnimMontage = MainWeapon->GetMontageForTag(DS1GameplayTags::Character_Action_ParriedHit);
		const float Delay = PlayAnimMontage(ParriedAnimMontage) + 1.f;

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]()
			{
				FGameplayTagContainer CheckTags;
				CheckTags.AddTag(DS1GameplayTags::Character_State_Death);
				if (StateComponent->IsCurrentStateEqualToAny(CheckTags) == false)
				{
					StateComponent->ClearState();
				}
			});
		GetWorld()->GetTimerManager().SetTimer(ParriedDelayTimerHandle, TimerDelegate, Delay, false);
	}

	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(
	// 		/* Key */ -1,
	// 		/* Duration */ 5.0f,
	// 		/* Color */ FColor::Green,
	// 		/* Message */ FString(TEXT("패링함"))
	// 	);
	// }
	
	//AttributeComponent->SetBaseStamina(0.f);
}

void ADS1Enemy::ToggleHealthBarVisibility(bool bVisibility)
{
	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(bVisibility);
	}
}

void ADS1Enemy::SeesTarget(AActor* InTargetActor)
{
}
