// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInstancedStaticMeshComponent* InstancedMeshComponent;
};
