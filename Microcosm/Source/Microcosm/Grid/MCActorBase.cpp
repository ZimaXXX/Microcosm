// Fill out your copyright notice in the Description page of Project Settings.


#include "MCActorBase.h"
#include "MCCommons.h"
#include "Hex/HexGrid.h"

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
}

FIntVector AMCActorBase::MoveTo(FIntVector InTargetPosition)
{
	FTransform HexTransform = HexGrid->GetTransformFromHexPosition(InTargetPosition);
	SetActorLocation(HexTransform.GetLocation());
	PositionOnGrid = InTargetPosition;
	return InTargetPosition;
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

