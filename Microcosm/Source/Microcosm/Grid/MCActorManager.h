// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MCActorBase.h"
#include "GameFramework/Actor.h"
#include "MCActorManager.generated.h"

class AHexGrid;



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
	TArray<FIntVector> ApplyMovement(TArray<AMCActorBase*> InMCActors);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnWorldStepTick(int32 StepTickCount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	AHexGrid* HexGrid = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	TArray<FMCActorConfig> BlueActorConfigs;
	TArray<FMCActorAppliedConfig> AppliedBlueActorConfigs;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	TArray<FMCActorConfig> RedActorConfigs;
	TArray<FMCActorAppliedConfig> AppliedRedActorConfigs;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	TSubclassOf<AMCActorBase> BlueMCActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	TSubclassOf<AMCActorBase> RedMCActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	int32 MaxHealth = 5;

	bool IsPositionOccupied(const FIntVector& InPositionToCheck, TArray<FIntVector>& InOccupiedPositions);
	void ApplyMCActorTeamConfigs(ETeamType InTeam);
	
	TArray<AMCActorBase*> MCActors;
};
