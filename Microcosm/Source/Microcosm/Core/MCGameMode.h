// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MCGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MICROCOSM_API AMCGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	void OnWorldStepTimerTick();
	virtual void BeginPlay() override;
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	float WorldTimeStep = 0.1f;
};
