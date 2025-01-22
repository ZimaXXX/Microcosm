// Fill out your copyright notice in the Description page of Project Settings.


#include "MCHexGrid.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Microcosm/Core/MCGameMode.h"
#include "Microcosm/Core/MCWorldSettings.h"
#include "Microcosm/Grid/MCCommons.h"
#include "Microcosm/Interfaces/WorldStateInterface.h"


// Sets default values
AMCHexGrid::AMCHexGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	//Init ISM
	InstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMeshComponent"));
	InstancedMeshComponent->SetupAttachment(GetRootComponent());
	InstancedMeshComponent->NumCustomDataFloats = 3;
}

TArray<FIntVector> AMCHexGrid::GenerateHexGrid(int32 Radius)
{
	TArray<FIntVector> HexGrid;
	for (int32 q = -Radius; q <= Radius; ++q)
	{
		for (int32 r = -Radius; r <= Radius; ++r)
		{
			int32 s = -q - r;
			if (FMath::Abs(s) <= Radius)
			{
				HexGrid.Add(FIntVector(q, r, s));
			}
		}
	}
	return HexGrid;
}


void AMCHexGrid::CreateHexagonMap()
{
	check(InstancedMeshComponent);
	check(InstancedMeshComponent->GetStaticMesh());

	MapRadius = Cast<AMCWorldSettings>(GetWorldSettings())->GridRadius;
	float HolesRatio = Cast<AMCWorldSettings>(GetWorldSettings())->GridHolesRatio;
	
	if (MapRadius <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Map Radius invalid!"));
		return;
	}
	
	const FBoxSphereBounds Bounds = InstancedMeshComponent->GetStaticMesh()->GetBounds();
	const FVector HexBoxExtent = Bounds.BoxExtent;

	TArray<FIntVector> HexGrid = GenerateHexGrid(MapRadius);
	RemoveRandomHexesWithConnectivity(HexGrid, HolesRatio);

	float MeshX = 2 * HexBoxExtent.X;
	for (const FIntVector& Hex : HexGrid)
	{
		FVector WorldPosition = HexToWorldPosition(Hex, MeshX, MeshX);
		
		// Debug visualization
		//DrawDebugSphere(GetWorld(), WorldPosition, 10.0f, 12, FColor::Green, false, 10.0f);

		// Add instance to the ISM
		FTransform InstanceTransform(FRotator::ZeroRotator, WorldPosition, FVector::OneVector);
		const int32 InstanceIndex = InstancedMeshComponent->AddInstance(InstanceTransform);
		InstancedMeshComponent->SetCustomDataValue(InstanceIndex, 0, Hex.X);
		InstancedMeshComponent->SetCustomDataValue(InstanceIndex, 1, Hex.Y);
		InstancedMeshComponent->SetCustomDataValue(InstanceIndex, 2, Hex.Z);
	}
	float CameraOffset = HexBoxExtent.Y * 2 + 50.f;
	AdjustCameraForGrid(Camera, MeshX, MapRadius, CameraOffset);
	
	//LogHexData();
}

void AMCHexGrid::LogHexData()
{
	for (int32 Index = 0; Index < InstancedMeshComponent->GetNumInstances(); ++Index)
	{
		int32 Q = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3];
		int32 R = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3 + 1];
		int32 S = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3 + 2];
		UE_LOG(LogTemp, Display, TEXT("Hex Index %d Pos: Q: %d R: %d S: %d"), Index, Q, R, S);
	}
}

bool AMCHexGrid::IsHexAtPosition(FIntVector InPosition) const
{
	return GetHexAtPosition(InPosition) != INDEX_NONE;
}

int32 AMCHexGrid::GetHexAtPosition(FIntVector InPosition) const
{
	for (int32 Index = 0; Index < InstancedMeshComponent->GetNumInstances(); ++Index)
	{
		int32 Q = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3];
		if (InPosition.X == Q)
		{
			int32 R = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3 + 1];
			if (InPosition.Y == R)
			{
				int32 S = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3 + 2];
				if (InPosition.Z == S)
				{
					return Index;
				}
			}
		}
	}
	return INDEX_NONE;
}

FTransform AMCHexGrid::GetTransformFromHexPosition(FIntVector InPosition) const
{
	int32 Index = GetHexAtPosition(InPosition);
	FTransform Transform = FTransform();
	if (Index != INDEX_NONE)
	{
		InstancedMeshComponent->GetInstanceTransform(Index, Transform);
	}
	return Transform;
}

FTransform AMCHexGrid::GetTransformFromHexIndex(int32 InHexIndex) const
{
	FTransform Transform = FTransform();
	if (InHexIndex != INDEX_NONE && InHexIndex < InstancedMeshComponent->GetNumInstances())
	{
		InstancedMeshComponent->GetInstanceTransform(InHexIndex, Transform);
	}
	return Transform;
}

const TArray<FIntVector>* AMCHexGrid::GetEmptyHexPositions(TArray<FIntVector> ExcludedPositions, int32 Range, FIntVector InTestedPosition) const
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

int32 AMCHexGrid::GetHexDistance(FIntVector InTestedPosition, FIntVector InHexPosition) const
{
	int32 Distance = FMath::Max(
		FMath::Max(FMath::Abs(InTestedPosition.X - InHexPosition.X), FMath::Abs(InTestedPosition.Y - InHexPosition.Y)),
		FMath::Abs(InTestedPosition.Z - InHexPosition.Z)
	);
	return Distance;
}

bool AMCHexGrid::IsHexInRange(FIntVector InTestedPosition, FIntVector InHexPosition, int32 Range) const
{
	int32 Distance = GetHexDistance(InTestedPosition, InHexPosition);
	
	return Distance <= Range;
}

FVector AMCHexGrid::HexToWorldPosition(FIntVector Hex, float HexWidth, float HexHeight)
{
	float x = HexWidth * (Hex.X + 0.5f * Hex.Y);
	float y = HexHeight * 0.866f * Hex.Y; // 0.866 = sqrt(3)/2

	return FVector(x, y, 0); // Z remains 0 for a flat grid
}

bool AMCHexGrid::IsHexPassable(FIntVector InTestedPosition)
{
	return IsHexAtPosition(InTestedPosition) && !OccupiedPositions.Contains(InTestedPosition);
}

TArray<FIntVector> AMCHexGrid::GetHexNeighbors(FIntVector InTestedPosition)
{
		TArray<FIntVector> Neighbors;
    	for (const FIntVector& Direction : HexDirections)
    	{
    		FIntVector TestedNeighbor = InTestedPosition + Direction;
    		Neighbors.Add(TestedNeighbor);		
    	}
    	return Neighbors;
}

void AMCHexGrid::RemoveRandomHexesWithConnectivity(TArray<FIntVector>& HexGrid, float Ratio)
{
	Ratio = FMath::Clamp(Ratio, 0.f, 1.f);
	if (Ratio == 0.f)//no removals
	{
		return;
	}
	if (IWorldStateInterface* WorldStateInterface = Cast<IWorldStateInterface>(GetWorld()->GetAuthGameMode()))
	{
		FRandomStream& WorldRandomStream = WorldStateInterface->GetWorldRandomStream();
		int32 TotalHexes = HexGrid.Num();
		int32 NumToRemove = FMath::FloorToInt(TotalHexes * Ratio);
	
		// Shuffle the hex grid
		for (int32 i = HexGrid.Num() - 1; i > 0; --i)
		{
			int32 RandomIndex = WorldRandomStream.RandRange(0, i);
			HexGrid.Swap(i, RandomIndex);
		}

		// Remove hexes one by one, ensuring connectivity
		for (int32 i = 0; i < NumToRemove; ++i)
		{
			FIntVector HexToRemove = HexGrid[i];

			// Simulate removal
			TArray<FIntVector> TestGrid = HexGrid;
			TestGrid.Remove(HexToRemove);

			// Check if the grid is still connected
			if (IsGridConnected(TestGrid, FIntVector(0, 0, 0)))
			{
				HexGrid.Remove(HexToRemove); // Apply removal
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Skipping removal of hex (%d, %d, %d) to maintain connectivity."), 
					   HexToRemove.X, HexToRemove.Y, HexToRemove.Z);
			}
		}
	}
}

bool AMCHexGrid::IsGridConnected(const TArray<FIntVector>& RemainingHexes, const FIntVector& StartHex)
{
	TArray<FIntVector> Visited;
	TArray<FIntVector> Queue = { StartHex };

	while (Queue.Num() > 0)
	{
		FIntVector Current = Queue[0];
		Queue.RemoveAt(0);

		// Skip if already visited
		if (Visited.Contains(Current))
		{
			continue;
		}

		// Mark as visited
		Visited.Add(Current);

		// Process neighbors
		for (const FIntVector& Direction : HexDirections) // Define HexDirections for your coordinate system
		{
			FIntVector Neighbor = Current + Direction;
			if (RemainingHexes.Contains(Neighbor) && !Visited.Contains(Neighbor))
			{
				Queue.Add(Neighbor);
			}
		}
	}

	// Grid is connected if all remaining hexes are visited
	return Visited.Num() == RemainingHexes.Num();
}

FIntVector AMCHexGrid::GetRandomEmptyHexPosition(TArray<FIntVector> ExcludedPositions, FIntVector InTestedPosition, int32 InRange) const
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

TArray<FIntVector> AMCHexGrid::FindPathWithAStar(FIntVector Start, FIntVector Goal)
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

void AMCHexGrid::AdjustCameraForGrid(ACameraActor* InCamera, int32 GridRadius, float HexSize, float InOffset)
{
	if (!InCamera) return;
	
	//Calculate grid dimensions
	float GridHeight = HexSize * 2 * FMath::Sqrt(3.f) * GridRadius;
	float GridWidth = HexSize * 2 * (GridRadius + 0.5f);

	//Get camera FOV (in radians)
	float VerticalFOV = FMath::DegreesToRadians(InCamera->GetCameraComponent()->FieldOfView);
	float AspectRatio = InCamera->GetCameraComponent()->AspectRatio;

	//Calculate camera distances
	float CameraDistanceVertical = (GridHeight / 2) / FMath::Tan(VerticalFOV / 2);
	float HorizontalFOV = 2 * FMath::Atan(FMath::Tan(VerticalFOV / 2) * AspectRatio);
	float CameraDistanceHorizontal = (GridWidth / 2) / FMath::Tan(HorizontalFOV / 2);

	//Use the larger distance
	float FinalCameraDistance = FMath::Max(CameraDistanceVertical, CameraDistanceHorizontal) + InOffset;

	//Set the camera position
	FVector CameraLocation = FVector(0.0f, 0.0f, FinalCameraDistance); // Adjust axes as needed
	InCamera->SetActorLocation(CameraLocation);

	//Aim at grid center
	InCamera->SetActorRotation(FRotator(-90.0f, 0.0f, 0.0f)); // Adjust rotation as needed
}