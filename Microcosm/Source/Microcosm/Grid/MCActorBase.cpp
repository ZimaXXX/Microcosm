// Fill out your copyright notice in the Description page of Project Settings.


#include "MCActorBase.h"
#include "MCCommons.h"
#include "Hex/HexGrid.h"
#include "Microcosm/Interfaces/MCManagerInfo.h"

AMCActorBase::AMCActorBase()
{
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

FIntVector AMCActorBase::MoveTo(FIntVector InTargetPosition)
{
	if (bHasAttackedThisTurn && !bCanMoveAfterAttack)
	{
		return PositionOnGrid;
	}
	FTransform HexTransform = HexGrid->GetTransformFromHexPosition(InTargetPosition);
	SetActorLocation(HexTransform.GetLocation());
	PositionOnGrid = InTargetPosition;
	return PositionOnGrid;
}

FIntVector AMCActorBase::ExecuteMovement()
{
	if (!IsValid(HexGrid))
	{
		UE_LOG(LogTemp, Error, TEXT("Hex Grid not available!"));
		return INVALID_GRID_POSITION;
	}
	//Is Enemy in Range
	if (MovementPattern == EMovementPattern::Random)
	{
		FIntVector TargetPosition = HexGrid->GetRandomEmptyHexPosition(HexGrid->OccupiedPositions, PositionOnGrid, 1);
		return MoveTo(TargetPosition);
	}
	return INVALID_GRID_POSITION;
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

