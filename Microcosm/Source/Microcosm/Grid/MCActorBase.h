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
	//Invalid value
	None = 0,
	Blue = 1,
	Red = 2
};

UENUM(BlueprintType)
enum class EMovementPattern : uint8
{
	//Invalid value
	None = 0,
	//Go to random empty neighbour each movement execution
	Random = 1,
	//Use AStar algorithm to find closest enemy and move one step ahead
	AStar = 2,
	//AStar + If no enemy move using Random
	AStarAndRandom = 3
};

USTRUCT(BlueprintType)
struct FMCActorConfig
{
	GENERATED_BODY();

	//Use Random Position on Grid
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseRandomPosition = true;
	//User Defined Position on Grid if not Randomized
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bUseRandomPosition"))
	FIntVector StartingPosition = INVALID_GRID_POSITION;

	//Movement Behaviour
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovementPattern MovementPattern = EMovementPattern::AStarAndRandom;

	//Speed where 1 is max and any bigger number means number of steps to wait
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpeedInWorldSteps = 1;

	//Randomize MaxHealth <1, MaxHealth>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseRandomHealth = true;
	//User Defined MaxHealth if not Randomized
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bUseRandomHealth"))
	int32 MaxHealth = 1;

	//Behaviour of movement after attacking
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanMoveAfterAttack = false;
	//Range of attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackRange = 1;
	//Power of attack
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
	//Delegates
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMCActorDeathDelegate OnMCActorDeathDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedDelegate OnHealthChangedDelegate;
	//Overrides
protected:
	virtual void Tick(float DeltaSeconds) override;
	//Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;
	
	//Constructor
public:
	AMCActorBase();
	
	//Methods
public:
	void Init(const FMCActorAppliedConfig& Config, AMCHexGrid* InHexGrid);
	void ExecuteMovement(FIntVector& OutNewPosition, FIntVector& OutPrevPosition, IMCManagerInfo* ManagerInfo);
	void ApplyDamage(int32 Damage);
	void OnAfterCombatCleanup();
	void ExecuteAttack(IMCManagerInfo* ManagerInfo);
		void OnNewTurn();
protected:
	void OrderMovementAnimation();
	FIntVector MoveTo(FIntVector InTargetPosition);
	void ApplyRandomMovementPattern();
	void ApplyAStarMovementPattern(IMCManagerInfo* ManagerInfo, bool bUseRandomPattern = false);
	void OnDeath();
	bool IsActorLocationMatchGridPosition(float Tolerance = UE_KINDA_SMALL_NUMBER) const;

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
//Properties
public:
	UPROPERTY(BlueprintReadOnly, Category = "State")
	FIntVector PositionOnGrid = INVALID_GRID_POSITION;
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
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	AMCHexGrid* HexGrid = nullptr;
};
