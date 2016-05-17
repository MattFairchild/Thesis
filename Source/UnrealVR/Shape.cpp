// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealVR.h"
#include "Shape.h"


// Sets default values
AShape::AShape()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject <USceneComponent>(TEXT("RootThing"));

	mesh = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("Shape's generated Mesh Component"));
	mesh->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void AShape::BeginPlay()
{
	Super::BeginPlay();
	
}
