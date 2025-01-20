// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hex/HexGrid.h"
#include "UObject/Object.h"
#include "MCActorBase.generated.h"

class IMCManagerInfo;

UENUM(BlueprintType)
enum class ETeamType : uint8
{
	None = 0,
	Blue = 1,
	Red = 2
};

UENUM(BlueprintType)
enum class EMovementPattern : uint8
{
	None = 0,
	Random = 1
};

USTRUCT(BlueprintType)
struct FMCActorConfig
{
	GENERATED_BODY();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseRandomPosition = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bUseRandomPosition"))
	FIntVector StartingPosition = INVALID_GRID_POSITION;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseRandomHealth = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bUseRandomHealth"))
	int32 MaxHealth = 1;
};

USTRUCT(BlueprintType)
struct FMCActorAppliedConfig
{
	GENERATED_BODY();
	UPROPERTY(BlueprintReadOnly)
	ETeamType TeamId = ETeamType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bUseRandomPosition"))
	FIntVector StartingPosition = INVALID_GRID_POSITION;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bUseRandomHealth"))
	int32 MaxHealth = 1;
};




/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class MICROCOSM_API AMCActorBase : public AActor
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMCActorDeathDelegate, AMCActorBase*, DeadMCActor);
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMCActorDeathDelegate OnMCActorDeathDelegate;
	
	AMCActorBase();
	
	UPROPERTY(BlueprintReadOnly)
	FIntVector PositionOnGrid = INVALID_GRID_POSITION;
	AHexGrid* HexGrid = nullptr;
	


	void Init(const FMCActorAppliedConfig& Config, AHexGrid* InHexGrid);
	FIntVector MoveTo(FIntVector InTargetPosition);
	FIntVector ExecuteMovement();
	void OnNewTurn();
	void OnDeath();
	void ApplyDamage(int32 Damage);
	void OnAfterCombatCleanup();
	void ExecuteAttack(IMCManagerInfo* ManagerInfo);


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;


	//Getters
public:
	bool IsDead() const
	{
		return CurrentHealth <= 0;
	}
	
	FIntVector GetPositionOnGrid() const
	{
		return PositionOnGrid;
	}

	ETeamType GetTeamId() const
	{
		return TeamId;
	}

	int32 GetMaxHealth() const
	{
		return MaxHealth;
	}

	 int32 GetAttackRange() const
	{
		return AttackRange;
	}

	int32 GetCurrentHealth() const
	{
		return CurrentHealth;
	}

protected:
	//Config
	ETeamType TeamId = ETeamType::None;
	int32 MaxHealth = 0;
	EMovementPattern MovementPattern = EMovementPattern::Random;

	//TODO Add to Init
	bool bCanMoveAfterAttack = false;
	int32 AttackRange = 1;
	int32 AttackPower = 1;
	
	//State
	bool bHasAttackedThisTurn = false;
	int32 CurrentHealth = 0;
};
