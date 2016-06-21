// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnActor.h"
#include "RPCManager.generated.h"

UCLASS()
class UNREALVR_API ARPCManager : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TSubclassOf<ASpawnActor> spawn;

public:	
	// Sets default values for this actor's properties
	ARPCManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	//RPCs
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_SpawnObject(FVector location);

	UFUNCTION(Unreliable, NetMulticast)
	void Multicast_SpawnObject(FVector location);
};
