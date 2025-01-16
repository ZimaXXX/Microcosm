// Fill out your copyright notice in the Description page of Project Settings.


#include "MCGameMode.h"

#include "MCGameState.h"

void AMCGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,           // Timer handle
		this,                  // Object that owns the timer
		&AMCGameMode::OnWorldStepTimerTick, // Function to call
		WorldTimeStep,                  // Time interval (in seconds)
		true                   // Looping? (true = yes)
	);
}

void AMCGameMode::OnWorldStepTimerTick()
{
	GetGameState<AMCGameState>()->OnWorldStepTimerTick();
}

