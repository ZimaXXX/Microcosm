// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Microcosm/Interfaces/WorldStateInterface.h"
#include "MCGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MICROCOSM_API AMCGameMode : public AGameModeBase, public IWorldStateInterface
{
	GENERATED_BODY()

public:
	AMCGameMode();

//Overrides
protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

//Methods
protected:
	void OnWorldStepTimerTick();
//Properties
protected:
	FTimerHandle TimerHandle;

	FRandomStream WorldRandomStream;

	//IWorldStateInterface
public:
	virtual FRandomStream& GetWorldRandomStream() override;

};
