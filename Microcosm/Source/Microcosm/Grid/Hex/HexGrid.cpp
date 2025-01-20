// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Microcosm/Core/MCGameMode.h"
#include "Microcosm/Grid/MCCommons.h"
#include "Microcosm/Interfaces/WorldStateInterface.h"


// Sets default values
AHexGrid::AHexGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	//Init ISM
	InstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMeshComponent"));
	InstancedMeshComponent->SetupAttachment(GetRootComponent());
	InstancedMeshComponent->NumCustomDataFloats = 3;
}


void AHexGrid::CreateHexagonMap()
{
	ensure(InstancedMeshComponent);
	ensure(InstancedMeshComponent->GetStaticMesh());
	const FBoxSphereBounds Bounds = InstancedMeshComponent->GetStaticMesh()->GetBounds();
	const FVector BoxExtent = Bounds.BoxExtent;

	for (int q = -MapRadius; q <= MapRadius; q++)
	{
		const int32 R1 = std::max(-MapRadius, -q - MapRadius);
		const int32 R2 = std::min(MapRadius, -q + MapRadius);
		
		for (int r = R1; r <= R2; r++)
		{
			if (HolesInMap.Num() > 0)
			{
				FIntVector PotentialHole = FIntVector(q, r, -q -r);
				if (HolesInMap.Contains(PotentialHole))
				{
					continue;
				}
			}
			const float LocationX = q * BoxExtent.X + static_cast<float>(r * 2 * BoxExtent.X);
			const float LocationY = static_cast<float>(q * 2 * BoxExtent.Y) - BoxExtent.Z * q;
			
			FVector Location = FVector(LocationX, LocationY, BoxExtent.Z);
			FRotator Rotation = FRotator::ZeroRotator;
			FTransform Transform = FTransform(Rotation, Location);
			
			const int32 InstanceIndex = InstancedMeshComponent->AddInstance(Transform);
			//Store position
			InstancedMeshComponent->SetCustomDataValue(InstanceIndex, 0, q);
			InstancedMeshComponent->SetCustomDataValue(InstanceIndex, 1, r);
			InstancedMeshComponent->SetCustomDataValue(InstanceIndex, 2, -q -r);
		}
	}
	//LogHexData();
}

void AHexGrid::LogHexData()
{
	for (int32 Index = 0; Index < InstancedMeshComponent->GetNumInstances(); ++Index)
	{
		int32 P = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3];
		int32 Q = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3 + 1];
		int32 R = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3 + 2];
		UE_LOG(LogTemp, Display, TEXT("Hex Index %d Pos: P: %d Q: %d R: %d"), Index, P, Q, R);
	}
}

bool AHexGrid::IsHexAtPosition(FIntVector InPosition) const
{
	return GetHexAtPosition(InPosition) != INDEX_NONE;
}

int32 AHexGrid::GetHexAtPosition(FIntVector InPosition) const
{
	for (int32 Index = 0; Index < InstancedMeshComponent->GetNumInstances(); ++Index)
	{
		int32 P = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3];
		if (InPosition.X == P)
		{
			int32 Q = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3 + 1];
			if (InPosition.Y == Q)
			{
				int32 R = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3 + 2];
				if (InPosition.Z == R)
				{
					return Index;
				}
			}
		}
	}
	return INDEX_NONE;
}

FTransform AHexGrid::GetTransformFromHexPosition(FIntVector InPosition) const
{
	int32 Index = GetHexAtPosition(InPosition);
	FTransform Transform = FTransform();
	if (Index != INDEX_NONE)
	{
		InstancedMeshComponent->GetInstanceTransform(Index, Transform);
	}
	return Transform;
}

FTransform AHexGrid::GetTransformFromHexIndex(int32 InHexIndex) const
{
	FTransform Transform = FTransform();
	if (InHexIndex != INDEX_NONE && InHexIndex < InstancedMeshComponent->GetNumInstances())
	{
		InstancedMeshComponent->GetInstanceTransform(InHexIndex, Transform);
	}
	return Transform;
}

const TArray<FIntVector>* AHexGrid::GetEmptyHexPositions(TArray<FIntVector> ExcludedPositions, int32 Range, FIntVector InTestedPosition) const
{
	TArray<FIntVector>* ValidPositions = new TArray<FIntVector>();
	for (int32 Index = 0; Index < InstancedMeshComponent->GetNumInstances(); ++Index)
	{
		FIntVector ISMPos = FIntVector::ZeroValue;
		ISMPos.X = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3];
		ISMPos.Y = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3 + 1];
		ISMPos.Z = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3 + 2];
		if (InTestedPosition == ISMPos)
		{
			continue;
		}
		bool IsExcluded = false;
		for (FIntVector ExcludedPosition : ExcludedPositions)
		{
			if (ISMPos == ExcludedPosition)
			{
				IsExcluded = true;
				ExcludedPositions.Remove(ExcludedPosition);
				break;
			}
		}
		if (!IsExcluded)
		{
			//Range checking
			if (Range > 0 && InTestedPosition != INVALID_GRID_POSITION)
			{
				if (IsHexInRange(InTestedPosition, ISMPos, Range))
				{
					ValidPositions->Add(ISMPos);
				}
			}
			else
			{
				ValidPositions->Add(ISMPos);
			}
		}		
	}
	return ValidPositions;
}

int32 AHexGrid::GetHexDistance(FIntVector InTestedPosition, FIntVector InHexPosition) const
{
	int32 Distance = FMath::Max(
		FMath::Max(FMath::Abs(InTestedPosition.X - InHexPosition.X), FMath::Abs(InTestedPosition.Y - InHexPosition.Y)),
		FMath::Abs(InTestedPosition.Z - InHexPosition.Z)
	);
	return Distance;
}

bool AHexGrid::IsHexInRange(FIntVector InTestedPosition, FIntVector InHexPosition, int32 Range) const
{
	int32 Distance = GetHexDistance(InTestedPosition, InHexPosition);
	
	return Distance <= Range;
}

bool AHexGrid::IsHexPassable(FIntVector InTestedPosition)
{
	return IsHexAtPosition(InTestedPosition) && !OccupiedPositions.Contains(InTestedPosition);
}

TArray<FIntVector> AHexGrid::GetPassableHexNeighbors(FIntVector InTestedPosition)
{
	TArray<FIntVector> Neighbors;
	for (const FIntVector& Direction : HexDirections)
	{
		FIntVector TestedNeighbor = InTestedPosition + Direction;
		if (IsHexPassable(TestedNeighbor))
		{
			Neighbors.Add(TestedNeighbor);
		}			
	}
	return Neighbors;
}

TArray<FIntVector> AHexGrid::GetHexNeighbors(FIntVector InTestedPosition)
{
		TArray<FIntVector> Neighbors;
    	for (const FIntVector& Direction : HexDirections)
    	{
    		FIntVector TestedNeighbor = InTestedPosition + Direction;
    		Neighbors.Add(TestedNeighbor);		
    	}
    	return Neighbors;
}


FIntVector AHexGrid::GetRandomEmptyHexPosition(TArray<FIntVector> ExcludedPositions, FIntVector InTestedPosition, int32 InRange) const
{
	const TArray<FIntVector>* ValidPositions = GetEmptyHexPositions(ExcludedPositions, InRange, InTestedPosition);
	if (ValidPositions->Num() == 0)
	{
		return INVALID_GRID_POSITION;
	}
	if (ValidPositions->Num() == 1)
	{
		return (*ValidPositions)[0];
	}
	FIntVector FinalPosition = INVALID_GRID_POSITION;
	if (IWorldStateInterface* WorldStateInterface = Cast<IWorldStateInterface>(GetWorld()->GetAuthGameMode()); ValidPositions && !ValidPositions->Num() == 0)
	{
		FRandomStream& WorldRandomStream = WorldStateInterface->GetWorldRandomStream();
		int32 Index = WorldRandomStream.RandRange(0, ValidPositions->Num() - 1);
		FinalPosition = (*ValidPositions)[Index];
	}
	return FinalPosition;
}

TArray<FIntVector> AHexGrid::FindPathWithAStar(FIntVector Start, FIntVector Goal)
{
	// Priority queue for open set
	TArray<FIntVector> OpenSet = { Start };

	// Maps to track scores
	TMap<FIntVector, int32> GScore;
	TMap<FIntVector, int32> FScore;
	TMap<FIntVector, FIntVector> CameFrom;

	// Initialize scores
	GScore.Add(Start, 0);
	FScore.Add(Start, GetHexDistance(Start, Goal));

	while (OpenSet.Num() > 0)
	{
		// Find node with lowest F-Score
		OpenSet.Sort([&](const FIntVector& A, const FIntVector& B) {
			return FScore[A] < FScore[B];
		});
		FIntVector Current = OpenSet[0];

		// Check if reached goal
		if (Current == Goal)
		{
			// Reconstruct path
			TArray<FIntVector> Path;
			while (CameFrom.Contains(Current))
			{
				Path.Add(Current);
				Current = CameFrom[Current];
			}
			Path.Add(Start);
			Algo::Reverse(Path);
			return Path;
		}

		OpenSet.RemoveAt(0);
		TArray<FIntVector> HexNeighbors = GetHexNeighbors(Current);
		for (const FIntVector& Neighbor : HexNeighbors)
		{
			if (Neighbor != Goal && !IsHexPassable(Neighbor))
			{
				continue;
			}
			int32 TentativeGScore = GScore[Current] + 1;

			if (!GScore.Contains(Neighbor) || TentativeGScore < GScore[Neighbor])
			{
				// Update scores and path
				CameFrom.Add(Neighbor, Current);
				GScore.Add(Neighbor, TentativeGScore);
				FScore.Add(Neighbor, TentativeGScore + GetHexDistance(Neighbor, Goal));

				if (!OpenSet.Contains(Neighbor))
				{
					OpenSet.Add(Neighbor);
				}
			}
		}
	}

	// Return empty path if no solution
	return {};
}