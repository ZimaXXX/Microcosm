// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MCManagerInfo.generated.h"

class AMCActorBase;
enum class ETeamType : uint8;
// This class does not need to be modified.
UINTERFACE()
class UMCManagerInfo : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MICROCOSM_API IMCManagerInfo
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual const TArray<AMCActorBase*> GetEnemyMCActorsInRange(FIntVector TestedPosition, ETeamType TeamId, int32 Range) = 0;
	virtual const AMCActorBase* GetClosestEnemyMCActor(FIntVector TestedPosition, ETeamType TeamId, int32& OutDistance) = 0;
};
