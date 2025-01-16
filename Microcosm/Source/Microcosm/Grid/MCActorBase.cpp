// Fill out your copyright notice in the Description page of Project Settings.


#include "MCActorBase.h"

AMCActorBase::AMCActorBase()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InstancedMeshComponent"));
	Mesh->SetupAttachment(GetRootComponent());
}
