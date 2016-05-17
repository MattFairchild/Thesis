// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Shape.generated.h"

UCLASS()
class UNREALVR_API AShape : public AActor
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, Category = "Shape")
	USceneComponent* mesh;

public:	
	// Sets default values for this actor's properties
	AShape();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

};
