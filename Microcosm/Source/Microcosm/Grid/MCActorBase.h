// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hex/MCHexGrid.h"
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
	Random = 1,
	AStar = 2,
	AStarAndRandom = 3//If no enemy move using random
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
	EMovementPattern MovementPattern = EMovementPattern::AStarAndRandom;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpeedInWorldSteps = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseRandomHealth = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bUseRandomHealth"))
	int32 MaxHealth = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanMoveAfterAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackRange = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackPower = 1;
};

USTRUCT(BlueprintType)
struct FMCActorAppliedConfig
{
	GENERATED_BODY();
	ETeamType TeamId = ETeamType::None;
	FIntVector StartingPosition = INVALID_GRID_POSITION;
	EMovementPattern MovementPattern = EMovementPattern::AStarAndRandom;
	int32 MaxHealth = 1;
	int32 SpeedInWorldSteps = 1;
	bool bCanMoveAfterAttack = false;
	int32 AttackRange = 1;
	int32 AttackPower = 1;
	
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class MICROCOSM_API AMCActorBase : public AActor
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMCActorDeathDelegate, AMCActorBase*, DeadMCActor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedDelegate, int32, CurrentHealth);
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMCActorDeathDelegate OnMCActorDeathDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedDelegate OnHealthChangedDelegate;
	

	AMCActorBase();
	
	UPROPERTY(BlueprintReadOnly)
	FIntVector PositionOnGrid = INVALID_GRID_POSITION;
	AMCHexGrid* HexGrid = nullptr;

	void Init(const FMCActorAppliedConfig& Config, AMCHexGrid* InHexGrid);
	bool IsActorLocationMatchGridPosition(float Tolerance = UE_KINDA_SMALL_NUMBER) const;
	void OrderMovementAnimation();
	virtual void Tick(float DeltaSeconds) override;
	FIntVector MoveTo(FIntVector InTargetPosition);
	void ExecuteMovement(FIntVector& OutNewPosition, FIntVector& OutPrevPosition, IMCManagerInfo* ManagerInfo);
	void ApplyRandomMovementPattern(
	);
	void ApplyAStarMovementPattern(IMCManagerInfo* ManagerInfo, bool bUseRandomPattern = false);
	void OnNewTurn();
	void OnDeath();
	
	void ApplyDamage(int32 Damage);
	void OnAfterCombatCleanup();
	void ExecuteAttack(IMCManagerInfo* ManagerInfo);


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void ReceiveOnDamage();
	
	UFUNCTION(BlueprintPure, Category = "Events")
	UWorld* BP_GetWorld();
	//Getters
public:
	UFUNCTION(BlueprintPure, Category = "State")
	bool IsDead() const
	{
		return CurrentHealth <= 0;
	}
	UFUNCTION(BlueprintPure, Category = "State")
	FIntVector GetPositionOnGrid() const
	{
		return PositionOnGrid;
	}
	UFUNCTION(BlueprintPure, Category = "State")
	ETeamType GetTeamId() const
	{
		return TeamId;
	}
	UFUNCTION(BlueprintPure, Category = "State")
	int32 GetMaxHealth() const
	{
		return MaxHealth;
	}
	UFUNCTION(BlueprintPure, Category = "State")
	int32 GetAttackRange() const
	{
		return AttackRange;
	}

	UFUNCTION(BlueprintPure, Category = "State")
	int32 GetCurrentHealth() const
	{
		return CurrentHealth;
	}

protected:
	//Config
	ETeamType TeamId = ETeamType::None;
	int32 MaxHealth = 0;
	EMovementPattern MovementPattern = EMovementPattern::None;
	int32 SpeedInWorldSteps = 1;
	bool bCanMoveAfterAttack = false;
	int32 AttackRange = 1;
	int32 AttackPower = 1;
	
	//State
	bool bHasAttackedThisTurn = false;
	int32 CurrentHealth = 0;
	FVector LerpInitialLocation = FVector::ZeroVector;
	int32 LastWorldStepTimeWithMovement = 0;
};
