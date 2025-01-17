// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Microcosm/Grid/MCCommons.h"
#include "HexGrid.generated.h"

class AHexBase;
UCLASS()
class MICROCOSM_API AHexGrid : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHexGrid();

	int32 MapRadius = 3;

	UFUNCTION(BlueprintCallable, Category="HexGrid")
	void CreateHexagonMap();

	void LogHexData();
	bool IsHexAtPosition(FIntVector InPosition) const;
	int32 GetHexAtPosition(FIntVector InPosition) const;
	FTransform GetTransformFromHexPosition(FIntVector InPosition) const;
	FTransform GetTransformFromHexIndex(int32 InHexIndex) const;
	FIntVector GetRandomEmptyHexPosition(TArray<FIntVector> ExcludedPositions, FIntVector InTestedPosition = INVALID_GRID_POSITION, int32 InRange = 0) const;

	const TArray<FIntVector>* GetEmptyHexPositions(TArray<FIntVector> ExcludedPositions, int32 Range = 0, FIntVector InTestedPosition = INVALID_GRID_POSITION) const;
	//FIntVector FindRandomEmptyHexInRange(FIntVector InTestedPosition, int32 InRange) const;
	bool IsHexInRange(FIntVector InTestedPosition, FIntVector InHexPosition, int32 Range) const;

	TArray<FIntVector> OccupiedPositions;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInstancedStaticMeshComponent* InstancedMeshComponent;
	

};
