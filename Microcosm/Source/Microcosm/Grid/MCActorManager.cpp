// Fill out your copyright notice in the Description page of Project Settings.


#include "MCActorManager.h"

#include "MCActorBase.h"
#include "Hex/HexGrid.h"


// Sets default values
AMCActorManager::AMCActorManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMCActorManager::SpawnTeam(ETeamType InTeam)
{
	TArray<FMCActorConfig>* MCActorConfigs = nullptr;
	TSubclassOf<AMCActorBase> MCActorClass = nullptr;
	if (InTeam == Blue)
	{
		MCActorConfigs = &BlueActorConfigs;
		MCActorClass = BlueMCActorClass;
	}
	else if (InTeam == Red)
	{
		MCActorConfigs = &RedActorConfigs;
		MCActorClass = RedMCActorClass;
	}
	else
	{
		return;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (FMCActorConfig Config: *MCActorConfigs)
	{
		FTransform HexTransform = HexGrid->GetTransformFromHexPosition(Config.StartingPosition);

		AMCActorBase* MCActor = GetWorld()->SpawnActorDeferred<AMCActorBase>(
			MCActorClass,
		HexTransform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		MCActor->FinishSpawning(HexTransform);
		// hex->InitializeHex(q, r, -q-r);
		// Map.Add(hex);
	}

}

void AMCActorManager::SpawnMCActors()
{
	ApplyMCActorTeamConfigs(Blue);
	ApplyMCActorTeamConfigs(Red);

	SpawnTeam(Blue);
	SpawnTeam(Red);
}

// Called when the game starts or when spawned
void AMCActorManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnMCActors();
}

bool AMCActorManager::IsPositionOccupied(const FIntVector& PositionToCheck, TArray<FIntVector>& OccupiedPositions)
{
	return OccupiedPositions.Contains(PositionToCheck);
}

void AMCActorManager::ApplyMCActorTeamConfigs(ETeamType InTeam)
{
	TArray<FIntVector> OccupiedPositions;
	TArray<FMCActorConfig>* MCActorConfigs = nullptr;
	if (InTeam == Blue)
	{
		MCActorConfigs = &BlueActorConfigs;
	}
	else if (InTeam == Red)
	{
		MCActorConfigs = &RedActorConfigs;
	}

	for (FMCActorConfig Config: *MCActorConfigs)
	{
		if (Config.StartingPosition == INVALID_GRID_POSITION)
		{
			continue;
		}
		if (Config.TeamId != InTeam)
		{
			Config.TeamId = InTeam;
		}
		if (!IsPositionOccupied(Config.StartingPosition, OccupiedPositions))
		{
			if (HexGrid->GetHexAtPosition(Config.StartingPosition))
			{
				OccupiedPositions.Add(Config.StartingPosition);
			}

		}
		
		AppliedBlueActorConfigs.Add(Config);
	}
}


