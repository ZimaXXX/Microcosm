// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "MCWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class MICROCOSM_API AMCWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	float WorldTimeStep = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	int32 WorldRandomSeed = 0;
};
