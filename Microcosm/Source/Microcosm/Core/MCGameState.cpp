// Fill out your copyright notice in the Description page of Project Settings.


#include "MCGameState.h"

#include "MCWorldSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//Executes only on server
void AMCGameState::OnWorldStepTimerTick()
{
	ensure(HasAuthority());
	//Executes on server and clients
	OnWorldStepTimerTick_Multicast(CurrentWorldStepCount + 1);	
}

void AMCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMCGameState, CurrentWorldStepCount);
}

void AMCGameState::OnWorldStepTimerTick_Multicast_Implementation(int32 InCurrentWorldStepCount)
{
	CurrentWorldStepCount = InCurrentWorldStepCount;
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