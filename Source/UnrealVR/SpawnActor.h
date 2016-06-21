// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnActor.generated.h"

UCLASS()
class UNREALVR_API ASpawnActor : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	class UStaticMeshComponent* mesh;

public:	
	// Sets default values for this actor's properties
	ASpawnActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
