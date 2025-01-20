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

void AMCActorManager::OnMCActorDeath(AMCActorBase* DeadMCActor)
{
	if (MCActors.Contains(DeadMCActor))
	{
		MCActors.Remove(DeadMCActor);
		UE_LOG(LogTemp, Warning, TEXT("Actor %s removed from MCActors"), *DeadMCActor->GetName());
	}
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
	RefreshMCActorsState();
	//Combat Phase
	TryToAttack();
	//Remove dead Actors
	AfterCombatCleanup();
	//Movement Phase
	ApplyMovement();
}

TArray<FIntVector> AMCActorManager::ApplyMovement()
{
	TArray<FIntVector> NewPositions;
	for (AMCActorBase* MCActor : MCActors)
	{
		FIntVector PrevPosition;
		FIntVector NewPosition;
		MCActor->ExecuteMovement(NewPosition, PrevPosition);
		if (NewPosition != PrevPosition)
		{
			NewPositions.Remove(PrevPosition);
			NewPositions.Add(NewPosition);
		}
	}
	return NewPositions;
}

void AMCActorManager::AfterCombatCleanup()
{
	for (int i = MCActors.Num() - 1; i >= 0; --i)//using reverse for loop to prevent issues with range changing inside the loop
	{
		MCActors[i]->OnAfterCombatCleanup();
	}
	UpdateHexGridInfo();
}

void AMCActorManager::TryToAttack()
{
	for (AMCActorBase* MCActor : MCActors)
	{
		MCActor->ExecuteAttack(this);
	}
	UpdateHexGridInfo();
}

void AMCActorManager::UpdateHexGridInfo()
{
	HexGrid->OccupiedPositions.Empty();
	for (const AMCActorBase* MCActor : MCActors)
	{
		if (IsValid(MCActor))
		{
			HexGrid->OccupiedPositions.Add(MCActor->PositionOnGrid);
		}

	}
}

void AMCActorManager::RefreshMCActorsState()
{
	for (AMCActorBase* MCActor : MCActors)
	{
		if (IsValid(MCActor))
		{
			MCActor->OnNewTurn();
		}
	}
	UpdateHexGridInfo();
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
		MCActor->OnMCActorDeathDelegate.AddDynamic(this, &AMCActorManager::OnMCActorDeath);
		
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
				FRandomStream& WorldRandomStream = WorldStateInterface->GetWorldRandomStream();
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
				break;
			case ETeamType::Red:
				AppliedRedActorConfigs.Add(AppliedConfig);
				break;
			default:
				break;
		}
	}
}

const TArray<AMCActorBase*> AMCActorManager::GetEnemyMCActorsInRange(FIntVector TestedPosition, ETeamType TeamId, int32 Range)
{
	TArray<AMCActorBase*> EnemyMCActors;
	if (TeamId == ETeamType::None || Range <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid team or Range!"));
		return {};
	}
	for (AMCActorBase* MCActor: MCActors)
	{
		if (MCActor->GetTeamId() != TeamId && !MCActor->IsDead())
		{
			if (bool IsHexInRange = HexGrid->IsHexInRange(TestedPosition, MCActor->GetPositionOnGrid(), Range))
			{
				EnemyMCActors.Add(MCActor);
			}
		}
	}
	return EnemyMCActors;
}


