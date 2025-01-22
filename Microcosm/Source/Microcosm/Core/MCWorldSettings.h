// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "MCWorldSettings.generated.h"

struct FMCActorConfig;
/**
 * 
 */
UCLASS()
class MICROCOSM_API AMCWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	//Speed of the simulation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config | World")
	float WorldTimeStep = 0.1f;

	//Seed for deterministic simulation. O means random.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config | World")
	int32 WorldRandomSeed = 0;

	//Radius of Hex Grid
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config | Grid")
	int32 GridRadius = 4;

	//Ratio of Holes generated in Grid. Connectivity is guaranteed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config | Grid")
	float GridHolesRatio = 0.1f;

	//Configs for Blue MCActors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config | MCActors")
	TArray<FMCActorConfig> BlueActorConfigs;

	//Configs for Red MCActors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config | MCActors")
	TArray<FMCActorConfig> RedActorConfigs;
};
