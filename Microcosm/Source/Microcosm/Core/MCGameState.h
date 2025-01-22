// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MCGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldStepTickDelegate, int32, StepTickCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAliveMCActorsCountUpdatedDelegate, int32, BlueCount, int32, RedCount);

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

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAliveMCActorsCountUpdatedDelegate OnAliveMCActorsCountUpdatedDelegate;
	
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentWorldStepCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	double LastStepTimestamp = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State)
	int32 BlueCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State)
	int32 RedCount = 0;
	
	void OnWorldStepTimerTick();
	
	float GetCurrentTimeStepAlpha() const;
	
	void UpdateAliveMCActorsCount(int32 InBlueCount, int32 InRedCount);
};
