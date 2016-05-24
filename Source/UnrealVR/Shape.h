// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "Shape.generated.h"


UCLASS()
class UNREALVR_API AShape : public AStaticMeshActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
	TArray<UMaterialInterface*> mats;

	int currentMat;
public:	
	// Sets default values for this actor's properties
	AShape(const class FObjectInitializer &PCIP);

	virtual void BeginPlay();

	void switchColors();
};
