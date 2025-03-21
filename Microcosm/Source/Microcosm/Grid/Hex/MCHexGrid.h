﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "Microcosm/Grid/MCCommons.h"
#include "MCHexGrid.generated.h"

class AHexBase;
UCLASS()
class MICROCOSM_API AMCHexGrid : public AActor
{
	GENERATED_BODY()

	//Overrides	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInstancedStaticMeshComponent* InstancedMeshComponent;
	//Constructor
public:
	// Sets default values for this actor's properties
	AMCHexGrid();


	//Properties
public:
	TArray<FIntVector> OccupiedPositions;	
protected:
	const TArray<FIntVector> HexDirections = {
		FIntVector(1, -1, 0),  // East
		FIntVector(1, 0, -1),  // Northeast
		FIntVector(0, 1, -1),  // Northwest
		FIntVector(-1, 1, 0),  // West
		FIntVector(-1, 0, 1),  // Southwest
		FIntVector(0, -1, 1)   // Southeast
	};
	int32 MapRadius = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Config, meta = (AllowPrivateAccess = "true"))
	ACameraActor* Camera;

	//Methods
public:
	UFUNCTION(BlueprintCallable, Category="HexGrid")
	void CreateHexagonMap();

	bool IsHexAtPosition(FIntVector InPosition) const;
	int32 GetHexAtPosition(FIntVector InPosition) const;
	FTransform GetTransformFromHexPosition(FIntVector InPosition) const;
	FTransform GetTransformFromHexIndex(int32 InHexIndex) const;
	FIntVector GetRandomEmptyHexPosition(TArray<FIntVector> ExcludedPositions, FIntVector InTestedPosition = INVALID_GRID_POSITION, int32 InRange = 0) const;
	const TArray<FIntVector>* GetEmptyHexPositions(TArray<FIntVector> ExcludedPositions, int32 Range = 0, FIntVector InTestedPosition = INVALID_GRID_POSITION) const;
	int32 GetHexDistance(FIntVector InTestedPosition, FIntVector InHexPosition) const;
	bool IsHexInRange(FIntVector InTestedPosition, FIntVector InHexPosition, int32 Range) const;
	FVector HexToWorldPosition(FIntVector Hex, float HexWidth, float HexHeight);
	bool IsHexPassable(FIntVector InTestedPosition);
	TArray<FIntVector> GetHexNeighbors(FIntVector InTestedPosition);
	TArray<FIntVector> GenerateHexGrid(int32 Radius);
	int32 GetMaxRange() const
	{
		return 2 * MapRadius;
	}
	TArray<FIntVector> FindPathWithAStar(FIntVector Start, FIntVector Goal);
protected:
	bool IsGridConnected(const TArray<FIntVector>& RemainingHexes, const FIntVector& StartHex);
	void RemoveRandomHexesWithConnectivity(TArray<FIntVector>& HexGrid, float Ratio);
	void LogHexData();
	void AdjustCameraForGrid(ACameraActor* InCamera, int32 GridRadius, float HexSize, float InOffset);
};

