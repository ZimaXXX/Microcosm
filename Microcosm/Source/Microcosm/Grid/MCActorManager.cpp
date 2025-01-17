// Fill out your copyright notice in the Description page of Project Settings.


#include "MCActorManager.h"

#include "MCActorBase.h"
#include "MCCommons.h"
#include "GameFramework/GameModeBase.h"
#include "Hex/HexGrid.h"
#include "Microcosm/Core/MCGameState.h"
#include "Microcosm/Interfaces/WorldStateInterface.h"


// Sets default values
AMCActorManager::AMCActorManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMCActorManager::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		GetWorld()->GetGameState<AMCGameState>()->OnWorldStepTickDelegate.AddDynamic(this, &AMCActorManager::OnWorldStepTick);
		SpawnMCActors();
	}
}

void AMCActorManager::OnWorldStepTick(int32 StepTickCount)
{
	HexGrid->OccupiedPositions.Empty();
	HexGrid->OccupiedPositions.Append(ApplyMovement(MCActors));
}

TArray<FIntVector> AMCActorManager::ApplyMovement(TArray<AMCActorBase*> InMCActors)
{
	TArray<FIntVector> NewPositions;
	for (AMCActorBase* MCActor : InMCActors)
	{
		NewPositions.Add(MCActor->ExecuteMovement());
	}
	return NewPositions;
}

void AMCActorManager::SpawnTeam(ETeamType InTeam)
{
	TArray<FMCActorAppliedConfig>* MCActorConfigs = nullptr;
	TSubclassOf<AMCActorBase> MCActorClass = nullptr;
	if (InTeam == ETeamType::Blue)
	{
		MCActorConfigs = &AppliedBlueActorConfigs;
		MCActorClass = BlueMCActorClass;
	}
	else if (InTeam == ETeamType::Red)
	{
		MCActorConfigs = &AppliedRedActorConfigs;
		MCActorClass = RedMCActorClass;
	}
	else
	{
		return;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (FMCActorAppliedConfig Config: *MCActorConfigs)
	{
		FTransform HexTransform = HexGrid->GetTransformFromHexPosition(Config.StartingPosition);
		AMCActorBase* MCActor = GetWorld()->SpawnActorDeferred<AMCActorBase>(
			MCActorClass,
		HexTransform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		MCActor->Init(Config, HexGrid);
		MCActor->FinishSpawning(HexTransform);
		MCActors.Add(MCActor);
	}
}

void AMCActorManager::SpawnMCActors()
{
	ApplyMCActorTeamConfigs(ETeamType::Blue);
	ApplyMCActorTeamConfigs(ETeamType::Red);

	SpawnTeam(ETeamType::Blue);
	SpawnTeam(ETeamType::Red);
}

bool AMCActorManager::IsPositionOccupied(const FIntVector& InPositionToCheck, TArray<FIntVector>& InOccupiedPositions)
{
	return InOccupiedPositions.Contains(InPositionToCheck);
}

void AMCActorManager::ApplyMCActorTeamConfigs(ETeamType InTeam)
{
	//We need GameMode to implement WorldStateInterface
	ensure(GetWorld()->GetAuthGameMode()->GetClass()->ImplementsInterface(UWorldStateInterface::StaticClass()));
	
	const TArray<FMCActorConfig>* MCActorConfigs = nullptr;
	if (InTeam == ETeamType::Blue)
	{
		MCActorConfigs = &BlueActorConfigs;
	}
	else if (InTeam == ETeamType::Red)
	{
		MCActorConfigs = &RedActorConfigs;
	}

	for (FMCActorConfig Config: *MCActorConfigs)
	{
		FMCActorAppliedConfig* pAppliedConfig = new FMCActorAppliedConfig();
		FMCActorAppliedConfig AppliedConfig;
		if (!Config.bUseRandomPosition && Config.StartingPosition == INVALID_GRID_POSITION)
		{
			UE_LOG(LogTemp, Error, TEXT("Provided StartingPosition is INVALID!"));
			//Invalid data
			continue;
		}

		if (Config.bUseRandomPosition)
		{
			FIntVector Position = HexGrid->GetRandomEmptyHexPosition(HexGrid->OccupiedPositions);
			if (Position == INVALID_GRID_POSITION)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to find empty random Hex!"));
				continue;
			}
			AppliedConfig.StartingPosition = Position;//it's already validated
			HexGrid->OccupiedPositions.Add(AppliedConfig.StartingPosition);
		}
		else if (!IsPositionOccupied(Config.StartingPosition, HexGrid->OccupiedPositions))
		{
			if (HexGrid->IsHexAtPosition(Config.StartingPosition))
			{
				AppliedConfig.StartingPosition = Config.StartingPosition;
				HexGrid->OccupiedPositions.Add(AppliedConfig.StartingPosition);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid Hex Position!"));
				continue;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Position is occupied!"));
			continue;
		}
		
		if(Config.bUseRandomHealth)
		{
			if (IWorldStateInterface* WorldStateInterface = Cast<IWorldStateInterface>(GetWorld()->GetAuthGameMode()))
			{
				FRandomStream WorldRandomStream = WorldStateInterface->GetWorldRandomStream();
				AppliedConfig.MaxHealth = WorldRandomStream.RandRange(1, MaxHealth);
			}
		}
		else
		{
			AppliedConfig.MaxHealth = FMath::Clamp(Config.MaxHealth, 1, MaxHealth);
		}

		AppliedConfig.TeamId = InTeam;
		switch (InTeam)
		{
			case ETeamType::Blue:
				AppliedBlueActorConfigs.Add(AppliedConfig);
			case ETeamType::Red:
				AppliedRedActorConfigs.Add(AppliedConfig);
			default:;
		}
	}
}


