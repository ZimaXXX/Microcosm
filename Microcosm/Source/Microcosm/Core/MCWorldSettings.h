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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config | World")
	float WorldTimeStep = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config | World")
	int32 WorldRandomSeed = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config | Grid")
	int32 GridRadius = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config | Grid")
	float GridHolesRatio = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config | MCActors")
	TArray<FMCActorConfig> BlueActorConfigs;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config | MCActors")
	TArray<FMCActorConfig> RedActorConfigs;
};
