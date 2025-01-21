// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MCActorBase.h"
#include "GameFramework/Actor.h"
#include "Microcosm/Interfaces/MCManagerInfo.h"
#include "MCActorManager.generated.h"

class AHexGrid;

UCLASS(Blueprintable, BlueprintType)
class MICROCOSM_API AMCActorManager : public AActor, public IMCManagerInfo
{

	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMCActorManager();

protected:
	UFUNCTION()
	void OnMCActorDeath(AMCActorBase* DeadMCActor);
	void SpawnTeam(ETeamType InTeam);
	void SpawnMCActors();
	void ApplyMovement();
	void AfterCombatCleanup();
	void TryToAttack();
	void UpdateHexGridInfo();
	void RefreshMCActorsState();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnWorldStepTick(int32 StepTickCount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	AHexGrid* HexGrid = nullptr;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	//TArray<FMCActorConfig> BlueActorConfigs;
	TArray<FMCActorAppliedConfig> AppliedBlueActorConfigs;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	//TArray<FMCActorConfig> RedActorConfigs;
	TArray<FMCActorAppliedConfig> AppliedRedActorConfigs;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	TSubclassOf<AMCActorBase> BlueMCActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	TSubclassOf<AMCActorBase> RedMCActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	int32 MaxHealth = 5;

	bool IsPositionOccupied(const FIntVector& InPositionToCheck, TArray<FIntVector>& InOccupiedPositions);
	void ApplyMCActorTeamConfigs(ETeamType InTeam);

protected:
	UPROPERTY()
	TArray<AMCActorBase*> MCActors;

	//MCManagerInfo Interface
public:
	virtual const TArray<AMCActorBase*> GetEnemyMCActorsInRange(FIntVector TestedPosition, ETeamType TeamId, int32 Range) override;
	virtual const AMCActorBase* GetClosestEnemyMCActor(FIntVector TestedPosition, ETeamType TeamId, int32& OutDistance) override;
};
