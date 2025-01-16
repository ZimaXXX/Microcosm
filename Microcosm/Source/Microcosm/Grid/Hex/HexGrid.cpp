// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid.h"
#include "Components/InstancedStaticMeshComponent.h"


// Sets default values
AHexGrid::AHexGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	//Init ISM
	InstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMeshComponent"));
	InstancedMeshComponent->SetupAttachment(GetRootComponent());
	InstancedMeshComponent->NumCustomDataFloats = 3;
}


void AHexGrid::CreateHexagonMap()
{
	ensure(InstancedMeshComponent);
	ensure(InstancedMeshComponent->GetStaticMesh());
	const FBoxSphereBounds Bounds = InstancedMeshComponent->GetStaticMesh()->GetBounds();
	const FVector BoxExtent = Bounds.BoxExtent;

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

			const int32 InstanceIndex = InstancedMeshComponent->AddInstance(Transform);
			InstancedMeshComponent->SetCustomDataValue(InstanceIndex, 0, q);
			InstancedMeshComponent->SetCustomDataValue(InstanceIndex, 1, r);
			InstancedMeshComponent->SetCustomDataValue(InstanceIndex, 2, -q -r);
		}
	}
	LogHexData();
}

void AHexGrid::LogHexData()
{
	for (int32 Index = 0; Index < InstancedMeshComponent->GetNumInstances(); ++Index)
	{
		int32 P = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3];
		int32 Q = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3 + 1];
		int32 R = InstancedMeshComponent->PerInstanceSMCustomData[Index * 3 + 2];
		UE_LOG(LogTemp, Display, TEXT("Hex Index %d Pos: P: %d Q: %d R: %d"), Index, P, Q, R);
	}
}