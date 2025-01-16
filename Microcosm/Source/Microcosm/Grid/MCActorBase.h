// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MCActorBase.generated.h"

static const FIntVector INVALID_GRID_POSITION = FIntVector(MAX_int32);
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class MICROCOSM_API AMCActorBase : public AActor
{

	GENERATED_BODY()
public:
	AMCActorBase();
	
	UPROPERTY(BlueprintReadOnly)
	FIntVector PositionOnGrid = INVALID_GRID_POSITION;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	
};
