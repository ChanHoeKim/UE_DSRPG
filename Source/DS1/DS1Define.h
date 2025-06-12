// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define COLLISION_OBJECT_INTERACTION ECC_GameTraceChannel1
#define COLLISION_OBJECT_TARGETING ECC_GameTraceChannel2

//스테미너, 체력 어느 속성이 바뀌는지 타입 지정
UENUM(BlueprintType)
enum class EDS1AttributeType : uint8
{
	Stamina,
	Health,
};

//Hit 받은 방향 지정
UENUM(BlueprintType)
enum class EHitDirection : uint8
{
    Front,
    Back,
    Left,
    Right,
};

//락온 대상 변경 방향
UENUM(BlueprintType)
enum class ESwitchingDirection : uint8
{
    None,
    Left,
    Right,
};

//무기 타입
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    None,
    SwordShield,
    TwoHanded,
    MeleeFists
};

//무기 Collision 
UENUM(BlueprintType)
enum class EWeaponCollisionType : uint8
{
    RightWeaponCollision,
    LeftWeaponCollision,
};

UENUM(BlueprintType)
enum class EDS1AIBehavior : uint8
{
	Idle,
    Patrol,
    MeleeAttack,
    Approach,
    Stunned,
    Strafe,
};

UENUM(BlueprintType)
enum class EDS1ArmourType : uint8
{
	Chest,  // 상의
    Pants,  // 바지
    Boots,  // 신발
    Gloves, // 장갑
};

UENUM(BlueprintType)
enum class EDS1EquipmentType : uint8
{
	Weapon,
    Shield,
};

UENUM(BlueprintType)
enum class EDS1DamageType : uint8
{
	HitBack,
    KnockBack,
};