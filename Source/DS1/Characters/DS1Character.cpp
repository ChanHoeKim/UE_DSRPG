// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DS1Character.h"

#include "DS1GameplayTags.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Animation/DS1AnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/DS1CombatComponent.h"
#include "Components/DS1AttributeComponent.h"
#include "Components/DS1PotionInventoryComponent.h"
#include "Components/DS1StateComponent.h"
#include "Components/DS1TargetingComponent.h"
#include "Components/TextBlock.h"
#include "Engine/DamageEvents.h"
#include "Equipments/DS1FistWeapon.h"
#include "Equipments/DS1Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/DS1Interact.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundCue.h"
#include "UI/DS1EscWidget.h"
#include "UI/DS1PlayerHUDWidget.h"

ADS1Character::ADS1Character()
{
	//PrimaryActorTick.bCanEverTick = true;

	/* 컨트롤러의 회전을 따라가지 않게 함 */
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//움직임 방향과 캐릭터의 몸통 방향을 일치시킴
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//움직임 방향과 몸통방향을 일치시킬 때 회전하는 속도
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	/* 이동, 감속 속도 */
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	/*스프링 암 컴포넌트 부착 설정*/
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	CameraBoom->bUsePawnControlRotation = true; //마우스를 움직이면 이동

	/*카메라 컴포넌트 부착 설정*/
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	/* 메타 휴먼 바디 설정 */
	TorsoMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Torso"));
	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs"));
	FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feet"));
	TorsoMesh->SetupAttachment(GetMesh());
	LegsMesh->SetupAttachment(GetMesh());
	FeetMesh->SetupAttachment(GetMesh());

	/*캐릭터의 능력 및 상태 등을 체크 할 수 있도록 컴포넌트 설정 및 부착*/
	AttributeComponent = CreateDefaultSubobject<UDS1AttributeComponent>(TEXT("Attribute"));
	StateComponent = CreateDefaultSubobject<UDS1StateComponent>(TEXT("State"));
	CombatComponent = CreateDefaultSubobject<UDS1CombatComponent>(TEXT("Combat"));
	TargetingComponent = CreateDefaultSubobject<UDS1TargetingComponent>(TEXT("Targeting"));
	PotionInventoryComponent = CreateDefaultSubobject<UDS1PotionInventoryComponent>(TEXT("PotionInventory"));
	
	// Character의 사망에 따른 상태 처리를 위해 함수 바인딩
	AttributeComponent->OnDeath.AddUObject(this, &ThisClass::OnDeath);
	
}

void ADS1Character::BeginPlay()
{
	Super::BeginPlay();

	/* 시작 메뉴일 때에는 해당 처리를 하지 않도록 체크 */
	const FString CurrentMap = GetWorld()->GetMapName();
	if (!CurrentMap.Contains(TEXT("Start")))
	{
		/* Player HUD 생성 및 화면 표출 */
		if (PlayerHUDWidgetClass)
		{
			PlayerHUDWidget = Cast<UDS1PlayerHUDWidget>(CreateWidget(GetWorld(), PlayerHUDWidgetClass));
		
			if (PlayerHUDWidget)
			{
				//UE_LOG(LogTemp, Log, TEXT("PlayerHUDWidgetClass 생성"));
				PlayerHUDWidget->AddToViewport();
			}
		}

		if (EscWidgetClass)
		{
			EscWidget = Cast<UDS1EscWidget>(CreateWidget<UUserWidget>(GetWorld(),EscWidgetClass));
			EscWidget->SetIsFocusable(true);
		}
		
		/* 게임 시작 시 주먹 무기 장착 */
		if (FistWeaponClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			ADS1FistWeapon* FistWeapon = GetWorld()->SpawnActor<ADS1FistWeapon>(FistWeaponClass, GetActorTransform(), SpawnParams);
			FistWeapon->EquipItem();
		}
		
	}
}

void ADS1Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

//Pawn의 Controller가 변경이 될 때 호출되는 함수
void ADS1Character::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			//MappingContext 바인딩
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Controller가 Pawn 또는 Character에 빙의 할 때 호출되는 함수
// 입력을 담당하는 함수를 바인딩하는 함수
void ADS1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 움직임
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

		// 카메라
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		// 질주
		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Triggered, this, &ThisClass::Sprinting);
		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Completed, this, &ThisClass::StopSprint);

		// 구르기
		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Canceled, this, &ThisClass::Rolling);

		// 상호작용
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::Interact);

		// 전투 활성/비활성
		EnhancedInputComponent->BindAction(ToggleCombatAction, ETriggerEvent::Started, this, &ThisClass::ToggleCombat);

		// 전투 상태로 전환.
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::AutoToggleCombat);

		/* 공격 */
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &ThisClass::Attack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ThisClass::SpecialAttack);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ThisClass::HeavyAttack);

		/* 락온 */
		EnhancedInputComponent->BindAction(LockOnTargetAction, ETriggerEvent::Started, this, &ThisClass::LockOnTarget);
		EnhancedInputComponent->BindAction(LeftTargetAction, ETriggerEvent::Started, this, &ThisClass::LeftTarget);
		EnhancedInputComponent->BindAction(RightTargetAction, ETriggerEvent::Started, this, &ThisClass::RightTarget);

		/* 방어 */ 
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &ThisClass::Blocking);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &ThisClass::BlockingEnd);

		// 패링
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &ThisClass::Parrying);

		// 포션 마시기
		EnhancedInputComponent->BindAction(ConsumeAction, ETriggerEvent::Started, this, &ThisClass::Consume);

		// Esc
		EnhancedInputComponent->BindAction(EscAction, ETriggerEvent::Triggered, this, &ThisClass::Esc);

		//죽었을 때 아무키나 눌러서 다시 시작
		EnhancedInputComponent->BindAction(EnterAction, ETriggerEvent::Triggered, this, &ThisClass::DeadStatePressedEnter);
	}
}

void ADS1Character::Esc()
{
	// 게임 일시 정지
	UGameplayStatics::SetGamePaused(GetWorld(), true);  

	//Esc 위젯 화면 표출
	if (EscWidget)
	{
		EscWidget->AddToViewport(999);
		
		// UI 모드로 전환
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			PC->bShowMouseCursor = true;
			
			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			// 안전하게 위젯 포커스를 설정
			InputMode.SetWidgetToFocus(EscWidget->GetCachedWidget());
			PC->SetInputMode(InputMode);
		}
	}
}

//죽음 상태인지 체크하는 함수
bool ADS1Character::IsDeath() const
{
	check(StateComponent)

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(DS1GameplayTags::Character_State_Death);

	return StateComponent->IsCurrentStateEqualToAny(CheckTags);
}

//캐릭터 몸통 활성화/비활성화 처리하는 함수
void ADS1Character::SetBodyPartActive(const EDS1ArmourType ArmourType, const bool bActive) const
{
	switch (ArmourType) {
	case EDS1ArmourType::Chest:
		TorsoMesh->SetVisibility(bActive);
		TorsoMesh->SetActive(bActive);
		break;
	case EDS1ArmourType::Pants:
		LegsMesh->SetVisibility(bActive);
		LegsMesh->SetActive(bActive);
		break;
	case EDS1ArmourType::Boots:
		FeetMesh->SetVisibility(bActive);
		FeetMesh->SetActive(bActive);
		break;
	case EDS1ArmourType::Gloves:
		break;
	}
}

//Character가 데미지를 받을 때 호출되는 함수
//ApplyDamage(ApplyPointDamage, ApplyRadialDamage) 함수의 Target이 호출하는 함수
float ADS1Character::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//ApplyDamage() 함수를 통해 받은 처음 데미지 양
	float  ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	//데미지 받을 타이밍에 구르기 상태인지 체크
	if (IsInvincible())
	{
		UE_LOG(LogTemp, Warning, TEXT("구르기 무적 타이밍"));
		return 0.f;
	}

	check(AttributeComponent); // 속성(체력, 스테미너) 컴포넌트 있는지 체크
	check(StateComponent);     // 전투 관련 컴포넌트 있는지 체크 

	// 포션 마시는 행동 중인지 체크 및 후조치
	IsAttackedWhileDrinkingPotion();

	// 가해자가 캐릭터의 정면에 있는지 체크
	bFacingEnemy = UKismetMathLibrary::InRange_FloatFloat(GetDotProductTo(EventInstigator->GetPawn()), -0.1f, 1.f);

	
	// 패링 시도를 했고, 패링 성공 조건에 맞는지 체크
	if (ParriedAttackSucceed())
	{
		//가해자 Pawn이 WasParried() 함수를 구현했는지 체크
		if (IDS1CombatInterface* CombatInterface = Cast<IDS1CombatInterface>(EventInstigator->GetPawn()))
		{
			//패링 시도
			CombatInterface->Parried();

			// @Incomplete : 패링 판정 다시 확인
			ADS1Weapon* MainWeapon = CombatComponent->GetMainWeapon();
			if (IsValid(MainWeapon))
			{
				FVector Location = MainWeapon->GetActorLocation();
				ShieldBlockingEffect(Location);
			}
		}
		return ActualDamage;
	}


	// 방어 가능 체크
	if (CanPerformAttackBlocking())
	{
		//AttributeComponent->TakeDamageAmount(0.f);
		AttributeComponent->DecreaseStamina(20.f);// 스테미나 차감
		StateComponent->SetState(DS1GameplayTags::Character_State_Blocking); //방어 성공 액션을 위해
	}
	else
	{
		AttributeComponent->TakeDamageAmount(ActualDamage); // 체력 차감
		StateComponent->SetState(DS1GameplayTags::Character_State_Hit); // Hit 액션을 위해
	}

	// 움직임 막음
	StateComponent->ToggleMovementInput(false);
	
	//ApplyDamage를 줄 때 Point 형태로 주었는지 체크
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

		
		ImpactEffect(ImpactPoint);

		HitReaction(EventInstigator->GetPawn(), EDS1DamageType::HitBack);
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);

		const FVector HitLocation = RadialDamageEvent->Origin;

		ImpactEffect(HitLocation);

		HitReaction(EventInstigator->GetPawn(), EDS1DamageType::KnockBack);
	}

	return ActualDamage;
}

void ADS1Character::ImpactEffect(const FVector& Location)
{
	if (CanPerformAttackBlocking())
	{
		ShieldBlockingEffect(Location);
	}
	else
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
}

void ADS1Character::ShieldBlockingEffect(const FVector& Location) const
{
	if (BlockingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BlockingSound, Location);
	}

	if (BlockingParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlockingParticle, Location);
	}
}

void ADS1Character::HitReaction(const AActor* Attacker, const EDS1DamageType InDamageType)
{
	check(CombatComponent)

	if (CanPerformAttackBlocking())
	{
		if (UAnimMontage* BlockingMontage = CombatComponent->GetMainWeapon()->GetMontageForTag(DS1GameplayTags::Character_Action_BlockingHit))
		{
			PlayAnimMontage(BlockingMontage);
		}
	}
	else
	{
		if (InDamageType == EDS1DamageType::HitBack)
		{
			if (UAnimMontage* HitReactAnimMontage = CombatComponent->GetMainWeapon()->GetHitReactMontage(Attacker))
			{
				PlayAnimMontage(HitReactAnimMontage);
			}
		}
		else if (InDamageType == EDS1DamageType::KnockBack)
		{
			if (UAnimMontage* HitReactAnimMontage = CombatComponent->GetMainWeapon()->GetMontageForTag(DS1GameplayTags::Character_Action_KnockBackHit))
			{
				const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Attacker->GetActorLocation());
				SetActorRotation(FRotator(LookAtRotation));
				PlayAnimMontage(HitReactAnimMontage);
			}
		}
	}
}

void ADS1Character::OnDeath()
{
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

	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->PlayDeadStateFadeInText();
	}
}

void ADS1Character::NockDown(const AActor* InInstigator)
{
	//PlayAnimMontage(NockDownMontage);
	StateComponent->ToggleMovementInput(false);

	// Actor 위치에서 가해자 위치를 정면으로 보게하는 회전값
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), InInstigator->GetActorLocation());
	
	SetActorRotation(FRotator(LookAtRotation));
}

bool ADS1Character::IsMoving() const
{
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		return MovementComp->Velocity.Size2D() > 3.f && MovementComp->GetCurrentAcceleration() != FVector::Zero();
	}

	return false;
}

bool ADS1Character::CanToggleCombat() const
{
	check(StateComponent);

	if (IsValid(CombatComponent->GetMainWeapon()) == false)
	{
		return false;
	}

	if (CombatComponent->GetMainWeapon()->GetCombatType() == EWeaponType::MeleeFists)
	{
		return false;
	}

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(DS1GameplayTags::Character_State_Attacking);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Rolling);
	CheckTags.AddTag(DS1GameplayTags::Character_State_GeneralAction);

	return StateComponent->IsCurrentStateEqualToAny(CheckTags) == false;
}

bool ADS1Character::CanPerformAttack(const FGameplayTag& AttackTypeTag) const
{
	check(StateComponent)
	check(CombatComponent)
	check(AttributeComponent)

	if (IsValid(CombatComponent->GetMainWeapon()) == false)
	{
		return false;
	}

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(DS1GameplayTags::Character_State_Rolling);
	CheckTags.AddTag(DS1GameplayTags::Character_State_GeneralAction);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Hit);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Blocking);
	CheckTags.AddTag(DS1GameplayTags::Character_State_DrinkingPotion);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Parrying);

	const float StaminaCost = CombatComponent->GetMainWeapon()->GetStaminaCost(AttackTypeTag);

	return StateComponent->IsCurrentStateEqualToAny(CheckTags) == false
		&& CombatComponent->IsCombatEnabled()
		&& AttributeComponent->CheckHasEnoughStamina(StaminaCost);
}


void ADS1Character::Move(const FInputActionValue& Values)
{
	check(StateComponent);

	// 이동 입력 가능 상태인지 체크.
	if (StateComponent->MovementInputEnabled() == false)
	{
		return;
	}

	FVector2D MovementVector = Values.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotator(0, Rotation.Yaw, 0);

		const FVector ForwardVector = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
		const FVector RightVector = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardVector, MovementVector.Y);
		AddMovementInput(RightVector, MovementVector.X);
		
	}
}

void ADS1Character::Look(const FInputActionValue& Values)
{
	// LockedOn 상태에서는 입력 차단.
	if (TargetingComponent && TargetingComponent->IsLockOn())
	{
		return;
	}

	FVector2D LookDirection = Values.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookDirection.X);
		AddControllerPitchInput(LookDirection.Y);
	}
}

void ADS1Character::Sprinting()
{
	check(AttributeComponent);
	check(CombatComponent);

	if (CombatComponent->IsBlockingEnabled())
	{
		return;
	}

	if (AttributeComponent->CheckHasEnoughStamina(5.f) && IsMoving())
	{
		AttributeComponent->ToggleRecoveryStamina(false);

		GetCharacterMovement()->MaxWalkSpeed = SprintingWalkSpeed;

		AttributeComponent->DecreaseStamina(0.1f);

		bSprinting = true;
	}
	else
	{
		StopSprint();
	}
}

void ADS1Character::StopSprint()
{
	check(AttributeComponent);
	check(CombatComponent);

	if (CombatComponent->IsBlockingEnabled())
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;
	AttributeComponent->ToggleRecoveryStamina(true);
	bSprinting = false;
}

void ADS1Character::Rolling()
{
	check(AttributeComponent);
	check(StateComponent);

	if (StateComponent->MovementInputEnabled() == false)
	{
		return;
	}
	
	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(DS1GameplayTags::Character_State_Rolling);
	// CheckTags.AddTag(DS1GameplayTags::Character_State_Stunned);
	
	if (AttributeComponent->CheckHasEnoughStamina(15.f) && StateComponent->IsCurrentStateEqualToAny(CheckTags) == false)
	{
		// 스태미나 재충전 멈춤
		AttributeComponent->ToggleRecoveryStamina(false);

		// 이동입력 처리 무시.
		StateComponent->ToggleMovementInput(false);

		// 스태미나 차감.
		AttributeComponent->DecreaseStamina(15.f);

		// 구르기 애니메이션 재생
		PlayAnimMontage(RollingMontage);

		StateComponent->SetState(DS1GameplayTags::Character_State_Rolling);

		// 스태미나 재충전 시작
		AttributeComponent->ToggleRecoveryStamina(true, 1.5f);
	}
}

void ADS1Character::Interact()
{
	FHitResult OutHit;
	const FVector Start = GetActorLocation();
	const FVector End = Start;
	constexpr float Radius = 100.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_OBJECT_INTERACTION));

	TArray<AActor*> ActorsToIgnore;

	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		Start,
		End,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		//EDrawDebugTrace::ForDuration,
		EDrawDebugTrace::None,
		OutHit,
		true);

	if (bHit)
	{
		// 감지한 Actor와 Interaction
		if (AActor* HitActor = OutHit.GetActor())
		{
			if (IDS1Interact* Interaction = Cast<IDS1Interact>(HitActor))
			{
				Interaction->Interact(this);
			}
		}
	}
}

void ADS1Character::ToggleCombat()
{
	check(CombatComponent)
	check(StateComponent)

	if (CombatComponent)
	{
		if (const ADS1Weapon* Weapon = CombatComponent->GetMainWeapon())
		{
			if (CanToggleCombat())
			{
				StateComponent->SetState(DS1GameplayTags::Character_State_GeneralAction);

				if (CombatComponent->IsCombatEnabled())
				{
					PlayAnimMontage(Weapon->GetMontageForTag(DS1GameplayTags::Character_Action_Unequip));
				}
				else
				{
					PlayAnimMontage(Weapon->GetMontageForTag(DS1GameplayTags::Character_Action_Equip));
				}
			}
		}
	}
}

//현재 Combat 상태가 아닌데 공격버튼(마우스 왼쪽) 클릭하는지 체크
void ADS1Character::AutoToggleCombat()
{
	if (CombatComponent)
	{
		if (!CombatComponent->IsCombatEnabled())
		{
			ToggleCombat();
		}
	}
}

void ADS1Character::Attack()
{
	// 첫 번째 공격의 타입을 반환
	const FGameplayTag AttackTypeTag = GetAttackPerform();

	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

// 첫 번째 공격의 타입을 반환
// (보통 공격, 달리기 공격)
FGameplayTag ADS1Character::GetAttackPerform() const
{
	if (IsSprinting())
	{
		return DS1GameplayTags::Character_Attack_Running;
	}

	return DS1GameplayTags::Character_Attack_Light;
}

//첫 공격 시작
/* @Incomplete : 공격 타이밍 상세 설정 주석 처리(지금 안 씀) */
void ADS1Character::ExecuteComboAttack(const FGameplayTag& AttackTypeTag)
{
	//현재 공격상태가 아닐 때 = 첫 공격
	if (StateComponent->GetCurrentState() != DS1GameplayTags::Character_State_Attacking)
	{
		//if (bComboSequenceRunning && bCanComboInput == false)
		// if (bCanComboInput == false)
		// {
		// 	// 애니메이션은 끝났지만 아직 콤보 시퀀스가 유효할 때 - 추가 입력 기회
		ComboCounter++;
		// 	UE_LOG(LogTemp, Warning, TEXT("Additional input : Combo Counter = %d"), ComboCounter);
		// }
		// else
		// {
		// 	//UE_LOG(LogTemp, Warning, TEXT(">>> ComboSequence Started <<<"));
		// 	ResetCombo();
		// 	//bComboSequenceRunning = true;
		// }

		DoAttack(AttackTypeTag);
		GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
	}
	// else if (bCanComboInput)
	// {
	// 	// 콤보 윈도우가 열려 있을 때 - 최적의 타이밍
	// 	bSavedComboInput = true;
	// }
}

void ADS1Character::SpecialAttack()
{
	const FGameplayTag AttackTypeTag = DS1GameplayTags::Character_Attack_Special;

	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

void ADS1Character::HeavyAttack()
{
	AutoToggleCombat();

	const FGameplayTag AttackTypeTag = DS1GameplayTags::Character_Attack_Heavy;

	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

void ADS1Character::LockOnTarget()
{
	TargetingComponent->ToggleLockOn();
}

void ADS1Character::LeftTarget()
{
	TargetingComponent->SwitchingLockedOnActor(ESwitchingDirection::Left);
}

void ADS1Character::RightTarget()
{
	TargetingComponent->SwitchingLockedOnActor(ESwitchingDirection::Right);
}

void ADS1Character::Blocking()
{
	check(CombatComponent);
	check(StateComponent);
	
	if (CombatComponent->GetMainWeapon() && CombatComponent->GetShield() && CombatComponent->IsCombatEnabled())
	{
		if (CanPlayerBlockStance())
		{
			GetCharacterMovement()->MaxWalkSpeed = BlockingWalkSpeed;
			CombatComponent->SetBlockingEnabled(true);
			if (UDS1AnimInstance* AnimInstance = Cast<UDS1AnimInstance>(GetMesh()->GetAnimInstance()))
			{
				AnimInstance->UpdateBlocking(true);
				StateComponent->SetState(DS1GameplayTags::Character_State_Blocking);
			}
		}
	}
}

void ADS1Character::BlockingEnd()
{
	check(CombatComponent);
	check(StateComponent);

	if (CombatComponent->GetMainWeapon() && CombatComponent->GetShield() && CombatComponent->IsCombatEnabled())
	{
		CombatComponent->SetBlockingEnabled(false);
		if (UDS1AnimInstance* AnimInstance = Cast<UDS1AnimInstance>(GetMesh()->GetAnimInstance()))
		{
			AnimInstance->UpdateBlocking(false);
			StateComponent->ClearState();
		}
		GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;
	}
}

void ADS1Character::Parrying()
{
	check(CombatComponent);
	check(StateComponent);
	check(AttributeComponent);

	if (CanPerformParry())
	{
		if (const ADS1Weapon* MainWeapon = CombatComponent->GetMainWeapon())
		{
			UAnimMontage* ParryingMontage = MainWeapon->GetMontageForTag(DS1GameplayTags::Character_State_Parrying);

			StateComponent->ToggleMovementInput(false);
			AttributeComponent->ToggleRecoveryStamina(false);
			AttributeComponent->DecreaseStamina(10.f);

			PlayAnimMontage(ParryingMontage);

			AttributeComponent->ToggleRecoveryStamina(true, 1.5f);
		}
	}
}

void ADS1Character::Consume()
{
	if (!StateComponent)
	{
		return;
	}

	if (CanDrinkPotion())
	{
		StateComponent->SetState(DS1GameplayTags::Character_State_DrinkingPotion);
		PlayAnimMontage(DrinkingMontage);
		
	}
}

void ADS1Character::DeadStatePressedEnter()
{
	//UE_LOG(LogTemp, Warning, TEXT("Pressed key: %s"), *Key.ToString());
	
	if (IsDeath())
	{
		// 레벨 이동
		//UGameplayStatics::OpenLevel(this, FName("DevMap"));
		UGameplayStatics::OpenLevel(this, FName("Meadow_Environment_Set_Map")); 
	}
}


void ADS1Character::DoAttack(const FGameplayTag& AttackTypeTag)
{
	check(StateComponent)
	check(AttributeComponent)
	check(CombatComponent)

	if (const ADS1Weapon* Weapon = CombatComponent->GetMainWeapon())
	{
		StateComponent->SetState(DS1GameplayTags::Character_State_Attacking);
		StateComponent->ToggleMovementInput(false);
		CombatComponent->SetLastAttackType(AttackTypeTag);

		AttributeComponent->ToggleRecoveryStamina(false);

		UAnimMontage* Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter);
		if (!Montage) // 해당 콤보 카운트에 맞는 Montage 없음 = 콤보 끝
		{
			ComboCounter = 0;
			Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter);
		}

		PlayAnimMontage(Montage);

		const float StaminaCost = Weapon->GetStaminaCost(AttackTypeTag);
		AttributeComponent->DecreaseStamina(StaminaCost);
		AttributeComponent->ToggleRecoveryStamina(true, 1.5f);
	}
}



void ADS1Character::ResetCombo()
{
	UE_LOG(LogTemp, Warning, TEXT("Combo Reset"));

	//bComboSequenceRunning = false;
	bCanComboInput = false;
	//bSavedComboInput = false;
	ComboCounter = 0;
}

bool ADS1Character::CanPlayerBlockStance() const
{
	check(StateComponent);
	check(CombatComponent);
	check(AttributeComponent);

	if (IsSprinting())
	{
		return false;
	}

	ADS1Weapon* Weapon = CombatComponent->GetMainWeapon();
	if (!IsValid(Weapon))
	{
		return false;
	}

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(DS1GameplayTags::Character_State_Attacking);
	CheckTags.AddTag(DS1GameplayTags::Character_State_GeneralAction);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Hit);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Rolling);
	CheckTags.AddTag(DS1GameplayTags::Character_State_DrinkingPotion);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Parrying);

	return StateComponent->IsCurrentStateEqualToAny(CheckTags) == false &&
		Weapon->GetCombatType() == EWeaponType::SwordShield &&
		AttributeComponent->CheckHasEnoughStamina(1.f);
}

//방패로 막을 수 있는 상황인지 체크
bool ADS1Character::CanPerformAttackBlocking() const
{
	check(CombatComponent);
	check(AttributeComponent);

	// 적이 정면 && 막는 중인 상태 && 스테미너가 충분
	return bFacingEnemy && CombatComponent->IsBlockingEnabled() && AttributeComponent->CheckHasEnoughStamina(20.f);
}

bool ADS1Character::CanPerformParry() const
{
	check(StateComponent);
	check(CombatComponent);
	check(AttributeComponent);

	ADS1Weapon* MainWeapon = CombatComponent->GetMainWeapon();
	// if (!IsValid(MainWeapon))
	// {
	// 	return false;
	// }

	if (MainWeapon && CombatComponent->GetShield() && CombatComponent->IsCombatEnabled())
	{
		FGameplayTagContainer CheckTags;
		CheckTags.AddTag(DS1GameplayTags::Character_State_Attacking);
		CheckTags.AddTag(DS1GameplayTags::Character_State_Rolling);
		CheckTags.AddTag(DS1GameplayTags::Character_State_GeneralAction);
		CheckTags.AddTag(DS1GameplayTags::Character_State_Hit);
		CheckTags.AddTag(DS1GameplayTags::Character_State_Blocking);
		CheckTags.AddTag(DS1GameplayTags::Character_State_Death);
		CheckTags.AddTag(DS1GameplayTags::Character_State_Parrying);
		CheckTags.AddTag(DS1GameplayTags::Character_State_DrinkingPotion);

		return StateComponent->IsCurrentStateEqualToAny(CheckTags) == false &&
			MainWeapon->GetCombatType() == EWeaponType::SwordShield &&
			AttributeComponent->CheckHasEnoughStamina(1.f);
	}
	
	return false;
}

// 패링 시도를 했고, 패링에 성공
// @Incomplete : 패링 타이밍 계산 아직 안 함 = 정면에 있으면 무조건 패링
bool ADS1Character::ParriedAttackSucceed() const
{
	check(StateComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(DS1GameplayTags::Character_State_Parrying);

	return StateComponent->IsCurrentStateEqualToAny(CheckTags) && bFacingEnemy;
}

bool ADS1Character::CanDrinkPotion() const
{
	check(PotionInventoryComponent);
	check(StateComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(DS1GameplayTags::Character_State_Attacking);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Blocking);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Death);
	CheckTags.AddTag(DS1GameplayTags::Character_State_GeneralAction);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Hit);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Parrying);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Rolling);

	return PotionInventoryComponent->GetPotionQuantity() > 0 && StateComponent->IsCurrentStateEqualToAny(CheckTags) == false;
}

void ADS1Character::IsAttackedWhileDrinkingPotion() const
{
	check(StateComponent);
	
	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(DS1GameplayTags::Character_State_DrinkingPotion);

	//포션 마시는 상태인지 체크
	if (StateComponent->IsCurrentStateEqualToAny(CheckTags))
	{
		if (PotionInventoryComponent)
		{
			//손에 부착된 포션 제거
			PotionInventoryComponent->DespawnPotion();
		}
	}
}

// - Anim Notify에서 콤보 가능 부분을 체크하기 위한 함수
// - 지금 사용 안함 (Timer로 대신 체크)
void ADS1Character::EnableComboWindow()
{
	// bCanComboInput = true;
	//UE_LOG(LogTemp, Warning, TEXT("Combo Window Opened: Combo Counter = %d"), ComboCounter);
}

// - Anim Notify에서 콤보 가능 부분을 체크하기 위한 함수
// - 지금 사용 안함 (Timer로 대신 체크)
void ADS1Character::DisableComboWindow()
{
	// check(CombatComponent)
	//
	// bCanComboInput = false;

	// if (bSavedComboInput)
	// {
	// 	bSavedComboInput = false;
	// 	ComboCounter++;
	// 	UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed: Advancing to next combo = %d"), ComboCounter);
	// 	DoAttack(CombatComponent->GetLastAttackType());
	// }
	// else
	// {
	// 	//UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed: No input received"));
	// }
}

// 공격 종료 함수
void ADS1Character::AttackFinished(const float ComboResetDelay)
{
	//UE_LOG(LogTemp, Warning, TEXT("AttackFinished"));
	if (StateComponent)
	{
		StateComponent->ToggleMovementInput(true);
	}
	
	// ComboResetDelay 후에 다음 콤보 불가(다시 첫 공격부터 시작)
	GetWorld()->GetTimerManager().SetTimer(ComboResetTimerHandle, this, &ThisClass::ResetCombo, ComboResetDelay, false);
}

// Weapon으로 타격하기 위해 Collision 활성화
void ADS1Character::ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType)
{
	if (CombatComponent)
	{
		CombatComponent->GetMainWeapon()->ActivateCollision(WeaponCollisionType);
	}
}

// Weapon 타격 후 Collision 비활성화 
void ADS1Character::DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType)
{
	if (CombatComponent)
	{
		CombatComponent->GetMainWeapon()->DeactivateCollision(WeaponCollisionType);
	}
}

// 무적 상태 활성화 / 비활성화
void ADS1Character::ToggleInvincibilityFrames(const bool bEnabled)
{
	bEnabledInvincibilityFrames = bEnabled;
}

