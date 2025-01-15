// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid.h"

#include "HexBase.h"


// Sets default values
AHexGrid::AHexGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}


void AHexGrid::CreateHexagonMap()
{
	ensure(HexClass);
	//FVector Origin = FVector::ZeroVector;
	//FVector BoxExtent = FVector::ZeroVector;
	UStaticMeshComponent* StatickMeshComponent = Cast<UStaticMeshComponent>(
		HexClass.GetDefaultObject()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	FBoxSphereBounds Bounds = StatickMeshComponent->GetStaticMesh()->GetBounds();
	FVector BoxExtent = Bounds.BoxExtent;
	//XOffset = - BoxExtent.X;
	//YOffset = - BoxExtent.Z;
	for (int q = -MapRadius; q <= MapRadius; q++)
	{
		const int32 R1 = std::max(-MapRadius, -q - MapRadius);
		const int32 R2 = std::min(MapRadius, -q + MapRadius);
		
		for (int r = R1; r <= R2; r++)
		{
			const float LocationX = q * BoxExtent.X + static_cast<float>(r * 2 * BoxExtent.X);
			const float LocationY = static_cast<float>(q * 2 * BoxExtent.Y) - BoxExtent.Z * q;
			
			FVector Location = FVector(LocationX, LocationY, BoxExtent.Z);
			FRotator Rotation = FRotator::ZeroRotator;
			FTransform Transform = FTransform(Rotation, Location);
			
			AHexBase* hex = GetWorld()->SpawnActorDeferred<AHexBase>(HexClass, Transform);
			
			hex->Init(q, r, -q-r);
			Map.Add(hex);

			hex->FinishSpawning(Transform);
		}
	}
}

void AHexGrid::LogHexData()
{
	for (const AHexBase* HexItem : Map)
	{
		UE_LOG(LogTemp, Display, TEXT("Actor name & Cube co-ordinates: %s %s"), *HexItem->GetName(), *HexItem->PrintCoordinates());
	}
}