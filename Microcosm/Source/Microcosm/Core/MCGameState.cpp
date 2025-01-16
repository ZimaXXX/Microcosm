// Fill out your copyright notice in the Description page of Project Settings.


#include "MCGameState.h"

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
	OnWorldStepTickDelegate.Broadcast(CurrentWorldStepCount);
}
