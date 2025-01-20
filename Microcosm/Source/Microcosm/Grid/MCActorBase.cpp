// Fill out your copyright notice in the Description page of Project Settings.


#include "MCActorBase.h"
#include "Hex/HexGrid.h"
#include "Microcosm/Core/MCGameState.h"
#include "Microcosm/Interfaces/MCManagerInfo.h"

AMCActorBase::AMCActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InstancedMeshComponent"));
	Mesh->SetupAttachment(GetRootComponent());
}

void AMCActorBase::Init(const FMCActorAppliedConfig& Config, AHexGrid* InHexGrid)
{
	MaxHealth = Config.MaxHealth;
	TeamId = Config.TeamId;
	HexGrid = InHexGrid;
	PositionOnGrid = Config.StartingPosition;
	CurrentHealth = MaxHealth;
}

bool AMCActorBase::IsActorLocationMatchGridPosition() const
{
	return HexGrid->GetTransformFromHexPosition(PositionOnGrid).GetLocation().Equals(GetActorLocation());
}

void AMCActorBase::OrderMovementAnimation()
{
	if (!IsActorLocationMatchGridPosition())
	{
		LerpInitialLocation = GetActorLocation();
		SetActorTickEnabled(true);
	}
}

void AMCActorBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	float Alpha = GetWorld()->GetGameState<AMCGameState>()->GetCurrentTimeStepAlpha();
	FVector NewLocation = FMath::Lerp(LerpInitialLocation, HexGrid->GetTransformFromHexPosition(PositionOnGrid).GetLocation(), Alpha);
	SetActorLocation(NewLocation);
		
	if (Alpha >= 1.f || IsActorLocationMatchGridPosition())
	{
		LerpInitialLocation = FVector::ZeroVector;
		SetActorLocation(HexGrid->GetTransformFromHexPosition(PositionOnGrid).GetLocation());//ensure proper location without tolerance
		SetActorTickEnabled(false);
	}
}

FIntVector AMCActorBase::MoveTo(FIntVector InTargetPosition)
{
	FTransform HexTransform = HexGrid->GetTransformFromHexPosition(InTargetPosition);
	//SetActorLocation(HexTransform.GetLocation());
	PositionOnGrid = InTargetPosition;
	OrderMovementAnimation();
	return PositionOnGrid;
}

void AMCActorBase::ExecuteMovement(FIntVector& OutNewPosition, FIntVector& OutPrevPosition, IMCManagerInfo* ManagerInfo)
{
	FIntVector PrevPositionOnGrid = PositionOnGrid;
	OutPrevPosition = PrevPositionOnGrid;

	OutNewPosition = PositionOnGrid;
	if (!IsValid(HexGrid))
	{
		UE_LOG(LogTemp, Error, TEXT("Hex Grid not available!"));
		//return INVALID_GRID_POSITION;
		return;
	}
	if (bHasAttackedThisTurn && !bCanMoveAfterAttack)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot move after attack!"));
		return;
	}
	//Is Enemy in Range
	if (MovementPattern == EMovementPattern::Random)
	{
		FIntVector TargetPosition = HexGrid->GetRandomEmptyHexPosition(HexGrid->OccupiedPositions, PositionOnGrid, 1);
		MoveTo(TargetPosition);
	}
	else if (MovementPattern == EMovementPattern::AStar)
	{
		int32 Distance;
		const AMCActorBase* ClosestEnemy = ManagerInfo->GetClosestEnemyMCActor(PositionOnGrid, TeamId, Distance);
		if (IsValid(ClosestEnemy) && Distance > 1 && Distance > AttackRange)
		{
			TArray<FIntVector> Path = HexGrid->FindPathWithAStar(PositionOnGrid, ClosestEnemy->PositionOnGrid);
			if (Path.Num() > 1)
			{
				FIntVector TargetPosition = Path[1];
				MoveTo(TargetPosition);
			}
		}
	}
}

void AMCActorBase::OnNewTurn()
{
	bHasAttackedThisTurn = false;
}

void AMCActorBase::OnDeath()
{
	OnMCActorDeathDelegate.Broadcast(this);
	UE_LOG(LogTemp, Warning, TEXT("MCActor %s Got Destroyed!"), *this->GetName());
	Destroy();
}

void AMCActorBase::ApplyDamage(int32 Damage)
{
	CurrentHealth -= Damage;
}

void AMCActorBase::OnAfterCombatCleanup()
{
	if (CurrentHealth <= 0)
	{
		OnDeath();
	}
}

void AMCActorBase::ExecuteAttack(IMCManagerInfo* ManagerInfo)
{
	TArray<AMCActorBase*> EnemiesInRange = ManagerInfo->GetEnemyMCActorsInRange(PositionOnGrid, TeamId, AttackRange);
	if (EnemiesInRange.Num() > 0)
	{
		for (AMCActorBase* EnemyInRange : EnemiesInRange)
		{
			if (!EnemyInRange->IsDead())
			{
				UE_LOG(LogTemp, Warning, TEXT("MCActor %s Attacks: %s For damage: %d"), *this->GetName(), *EnemyInRange->GetName(), AttackPower);
				EnemyInRange->ApplyDamage(AttackPower);
				bHasAttackedThisTurn = true;
			}
		}	
	}
}

