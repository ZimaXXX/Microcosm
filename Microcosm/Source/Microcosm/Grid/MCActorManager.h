// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MCActorBase.h"
#include "GameFramework/Actor.h"
#include "MCActorManager.generated.h"

class AHexGrid;

UENUM(BlueprintType)
enum ETeamType : uint8
{
	None = 0,
	Blue = 1,
	Red = 2
};

USTRUCT(BlueprintType)
struct FMCActorConfig
{
	GENERATED_BODY();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector StartingPosition = INVALID_GRID_POSITION;
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<ETeamType> TeamId = None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth = 1;
};



UCLASS(Blueprintable, BlueprintType)
class MICROCOSM_API AMCActorManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMCActorManager();

protected:
	void SpawnTeam(ETeamType InTeam);
	void SpawnMCActors();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	AHexGrid* HexGrid = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	TArray<FMCActorConfig> BlueActorConfigs;
	TArray<FMCActorConfig> AppliedBlueActorConfigs;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	TArray<FMCActorConfig> RedActorConfigs;
	TArray<FMCActorConfig> AppliedRedActorConfigs;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	TSubclassOf<AMCActorBase> BlueMCActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	TSubclassOf<AMCActorBase> RedMCActorClass;
	
	bool IsPositionOccupied(const FIntVector& PositionToCheck, TArray<FIntVector>& OccupiedPositions);
	void ApplyMCActorTeamConfigs(ETeamType InTeam);
};
