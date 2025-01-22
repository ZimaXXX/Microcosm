// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MCActorBase.h"
#include "GameFramework/Actor.h"
#include "Microcosm/Interfaces/MCManagerInfo.h"
#include "MCActorManager.generated.h"

class AMCHexGrid;

UCLASS(Blueprintable, BlueprintType)
class MICROCOSM_API AMCActorManager : public AActor, public IMCManagerInfo
{

	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMCActorManager();
//Overrides
protected:
	virtual void BeginPlay() override;
	//Methods
protected:
	UFUNCTION()
	void OnMCActorDeath(AMCActorBase* DeadMCActor);
	void SpawnTeam(ETeamType InTeam);
	void SpawnMCActors();
	void ApplyMovement();
	void AfterCombatCleanup();
	void TryToAttack();
	void UpdateHexGridInfo();
	void UpdateMCActorsCount();
	void RefreshMCActorsState();
	bool IsPositionOccupied(const FIntVector& InPositionToCheck, TArray<FIntVector>& InOccupiedPositions);
	void ApplyMCActorTeamConfigs(ETeamType InTeam);
	
	UFUNCTION()
	void OnWorldStepTick(int32 StepTickCount);
	
//Properties
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	AMCHexGrid* HexGrid = nullptr;

	TArray<FMCActorAppliedConfig> AppliedBlueActorConfigs;

	TArray<FMCActorAppliedConfig> AppliedRedActorConfigs;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	TSubclassOf<AMCActorBase> BlueMCActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	TSubclassOf<AMCActorBase> RedMCActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	int32 MaxHealth = 5;
	
	UPROPERTY()
	TArray<AMCActorBase*> MCActors;

	//MCManagerInfo Interface
public:
	virtual const TArray<AMCActorBase*> GetEnemyMCActorsInRange(FIntVector TestedPosition, ETeamType TeamId, int32 Range) override;
	virtual const AMCActorBase* GetClosestEnemyMCActor(FIntVector TestedPosition, ETeamType TeamId, int32& OutDistance) override;
};
