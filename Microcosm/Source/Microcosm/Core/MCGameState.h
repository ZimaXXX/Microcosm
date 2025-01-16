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

	//Replicates to support joining clients
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 CurrentWorldStepCount = 0;

	void OnWorldStepTimerTick();

	//Using RPC for speed
	UFUNCTION(NetMulticast, Reliable)
	void OnWorldStepTimerTick_Multicast(int32 InCurrentWorldStepCount);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
