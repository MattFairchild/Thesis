// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealVR.h"
#include "RPCManager.h"


// Sets default values
ARPCManager::ARPCManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	spawn = ASpawnActor::StaticClass();
}

// Called when the game starts or when spawned
void ARPCManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARPCManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ARPCManager::Server_SpawnObject_Implementation(FVector location)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Server spawn called"));
	GetWorld()->SpawnActor <ASpawnActor>(spawn, location, GetActorRotation());
}

bool ARPCManager::Server_SpawnObject_Validate(FVector location)
{
	return true;
}