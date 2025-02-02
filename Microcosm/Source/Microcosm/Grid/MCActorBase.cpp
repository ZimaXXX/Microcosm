﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "MCActorBase.h"
#include "Hex/MCHexGrid.h"
#include "Microcosm/Core/MCGameState.h"
#include "Microcosm/Interfaces/MCManagerInfo.h"

AMCActorBase::AMCActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InstancedMeshComponent"));
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->CastShadow = false;
}

void AMCActorBase::Init(const FMCActorAppliedConfig& Config, AMCHexGrid* InHexGrid)
{
	MaxHealth = Config.MaxHealth;
	TeamId = Config.TeamId;
	HexGrid = InHexGrid;
	PositionOnGrid = Config.StartingPosition;
	CurrentHealth = MaxHealth;
	MovementPattern = Config.MovementPattern;
	SpeedInWorldSteps = Config.SpeedInWorldSteps;
	AttackPower = Config.AttackPower;
	AttackRange = Config.AttackRange;
	bCanMoveAfterAttack = Config.bCanMoveAfterAttack;
}

bool AMCActorBase::IsActorLocationMatchGridPosition(float Tolerance) const
{
	return HexGrid->GetTransformFromHexPosition(PositionOnGrid).GetLocation().Equals(GetActorLocation(), Tolerance);
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
	int32 CurrentWorldStep = GetWorld()->GetGameState<AMCGameState>()->CurrentWorldStepCount;
	float Alpha = GetWorld()->GetGameState<AMCGameState>()->GetCurrentTimeStepAlpha();
	if (CurrentWorldStep != MovementWorldStep || Alpha >= 1.f || IsActorLocationMatchGridPosition())
	{
		LerpInitialLocation = FVector::ZeroVector;
		SetActorLocation(HexGrid->GetTransformFromHexPosition(PositionOnGrid).GetLocation());//ensure proper location without tolerance
		SetActorTickEnabled(false);
	}
	else
	{
		FVector NewLocation = FMath::Lerp(LerpInitialLocation, HexGrid->GetTransformFromHexPosition(PositionOnGrid).GetLocation(), Alpha);
		SetActorLocation(NewLocation);
	}
}

FIntVector AMCActorBase::MoveTo(FIntVector InTargetPosition)
{
	FTransform HexTransform = HexGrid->GetTransformFromHexPosition(InTargetPosition);
	//SetActorLocation(HexTransform.GetLocation());
	PositionOnGrid = InTargetPosition;
	MovementWorldStep = GetWorld()->GetGameState<AMCGameState>()->CurrentWorldStepCount;
	OrderMovementAnimation();
	LastWorldStepTimeWithMovement = GetWorld()->GetGameState<AMCGameState>()->CurrentWorldStepCount;
	return PositionOnGrid;
}

void AMCActorBase::ExecuteMovement(FIntVector& OutNewPosition, FIntVector& OutPrevPosition, IMCManagerInfo* ManagerInfo)
{
	FIntVector PrevPositionOnGrid = PositionOnGrid;
	OutPrevPosition = PrevPositionOnGrid;
	OutNewPosition = PositionOnGrid;
	
	if (!IsValid(HexGrid))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hex Grid not available!"));
		return;
	}
	if (bHasAttackedThisTurn && !bCanMoveAfterAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot move after attack!"));
		return;
	}
	int32 CurrentWorldStepCount = GetWorld()->GetGameState<AMCGameState>()->CurrentWorldStepCount;
	if (LastWorldStepTimeWithMovement > 0 && CurrentWorldStepCount - LastWorldStepTimeWithMovement < SpeedInWorldSteps)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot move because of speed!"));
		return;
	}
	//Is Enemy in Range
	switch (MovementPattern)
	{
	case EMovementPattern::Random:
		ApplyRandomMovementPattern();
		break;
	case EMovementPattern::AStar:
		ApplyAStarMovementPattern(ManagerInfo, false);
		break;
	case EMovementPattern::AStarAndRandom:
		ApplyAStarMovementPattern(ManagerInfo, true);
		break;
	default:
		break;
	}
	OutNewPosition = PositionOnGrid;
}
void AMCActorBase::ApplyRandomMovementPattern()
{
	FIntVector TargetPosition = HexGrid->GetRandomEmptyHexPosition(HexGrid->OccupiedPositions, PositionOnGrid, 1);
	MoveTo(TargetPosition);
}
void AMCActorBase::ApplyAStarMovementPattern(IMCManagerInfo* ManagerInfo, bool bUseRandomPattern)
{
	int32 Distance;
	const AMCActorBase* ClosestEnemy = ManagerInfo->GetClosestEnemyMCActor(PositionOnGrid, TeamId, Distance);
	if (IsValid(ClosestEnemy) && Distance > 1)
	{
		TArray<FIntVector> Path = HexGrid->FindPathWithAStar(PositionOnGrid, ClosestEnemy->PositionOnGrid);
		if (Path.Num() > 1)
		{
			FIntVector TargetPosition = Path[1];
			MoveTo(TargetPosition);
		}
	}
	else if (bUseRandomPattern && !IsValid(ClosestEnemy))//execute Random if no enemy
	{
		FIntVector TargetPosition = HexGrid->GetRandomEmptyHexPosition(HexGrid->OccupiedPositions, PositionOnGrid, 1);
		MoveTo(TargetPosition);
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
	OnHealthChangedDelegate.Broadcast(CurrentHealth);
	ReceiveOnDamage();
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

UWorld* AMCActorBase::BP_GetWorld()
{
	return GetWorld();
}

