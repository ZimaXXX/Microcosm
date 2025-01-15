// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexBase.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class MICROCOSM_API AHexBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	int32 Q;
	UPROPERTY(BlueprintReadWrite)
	int32 R;
	UPROPERTY(BlueprintReadWrite)
	int32 S;
	
	// Sets default values for this actor's properties
	AHexBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;
public:
	virtual void Init(int32 q, int32 r, int32 s);

	virtual FString PrintCoordinates() const;
};
