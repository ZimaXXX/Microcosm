// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MCGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldStepTickDelegate, int32, StepTickCount);

/**
 * 
 */
UCLASS()
class MICROCOSM_API AMCGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWorldStepTickDelegate OnWorldStepTickDelegate;
	
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentWorldStepCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	double LastStepTimestamp = 0.f;

	void OnWorldStepTimerTick();
	
	float GetCurrentTimeStepAlpha() const;
};
