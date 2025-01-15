// Fill out your copyright notice in the Description page of Project Settings.


#include "HexBase.h"


// Sets default values
AHexBase::AHexBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());

}

void AHexBase::Init(int32 q, int32 r, int32 s)
{
	Q = q;
	R = r;
	S = s;
}

FString AHexBase::PrintCoordinates() const
{
	return FString::Printf(TEXT("(%d, %d, %d)"), Q, R, S);
}