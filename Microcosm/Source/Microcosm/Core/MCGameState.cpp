// Fill out your copyright notice in the Description page of Project Settings.


#include "MCGameState.h"

#include "MCWorldSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//Executes only on server
void AMCGameState::OnWorldStepTimerTick()
{
	CurrentWorldStepCount++;
	LastStepTimestamp = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	OnWorldStepTickDelegate.Broadcast(CurrentWorldStepCount);
}

float AMCGameState::GetCurrentTimeStepAlpha() const
{
	float Alpha = 1.f;
	if (LastStepTimestamp == 0.f)
	{
		return Alpha;
	}
	double WorldTimeStep = Cast<AMCWorldSettings>(GetWorldSettings())->WorldTimeStep;
	double CurrentTimestamp = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	Alpha = (CurrentTimestamp - LastStepTimestamp) / WorldTimeStep;
	//UE_LOG(LogTemp, Error, TEXT("Current Alpha: %f"), Alpha);
	return FMath::Clamp(Alpha, 0.f, 1.f);
}

void AMCGameState::UpdateAliveMCActorsCount(int32 InBlueCount, int32 InRedCount)
{
	BlueCount = InBlueCount;
	RedCount = InRedCount;
	OnAliveMCActorsCountUpdatedDelegate.Broadcast(BlueCount, RedCount);
}
