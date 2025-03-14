﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "MCGameMode.h"

#include "MCGameState.h"
#include "MCWorldSettings.h"

AMCGameMode::AMCGameMode()
{

}

void AMCGameMode::BeginPlay()
{
	Super::BeginPlay();	
	
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,           // Timer handle
		this,                  // Object that owns the timer
		&AMCGameMode::OnWorldStepTimerTick, // Function to call
		Cast<AMCWorldSettings>(GetWorldSettings())->WorldTimeStep,                  // Time interval (in seconds)
		true                   // Looping? (true = yes)
	);
}

void AMCGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Cast<AMCWorldSettings>(GetWorldSettings())->WorldRandomSeed == 0)
	{
		//if Seed is uninitialized we randomize
		Cast<AMCWorldSettings>(GetWorldSettings())->WorldRandomSeed = FMath::Rand32();
	}
	WorldRandomStream.Initialize(Cast<AMCWorldSettings>(GetWorldSettings())->WorldRandomSeed);
}

FRandomStream& AMCGameMode::GetWorldRandomStream()
{
	return WorldRandomStream;
}

void AMCGameMode::OnWorldStepTimerTick()
{
	GetGameState<AMCGameState>()->OnWorldStepTimerTick();
}

