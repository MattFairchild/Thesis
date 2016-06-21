// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealVR.h"
#include "SpawnActor.h"


// Sets default values
ASpawnActor::ASpawnActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshOb(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_QuadPyramid.Shape_QuadPyramid'"));
	
	if (StaticMeshOb.Succeeded())
	{
		mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnableMesh)"));
		mesh->SetStaticMesh(StaticMeshOb.Object);
	}
}

// Called when the game starts or when spawned
void ASpawnActor::BeginPlay()
{
	Super::BeginPlay();
	
}

