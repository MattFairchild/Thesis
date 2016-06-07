// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "Engine.h"
#include "NET/UnrealNetwork.h"
#include "Shape.generated.h"


UCLASS()
class UNREALVR_API AShape : public AStaticMeshActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")

	TArray<UMaterialInterface*> mats;

	UPROPERTY(ReplicatedUsing=UpdateMaterial)
	int currentMat;
public:	
	// Sets default values for this actor's properties
	AShape(const class FObjectInitializer &PCIP);

	virtual void BeginPlay();

	UFUNCTION(Reliable, Server, WithValidation)
	void setShapePosition(FVector location);

	UFUNCTION(Reliable, Server, WithValidation)
	void switchColors();

	UFUNCTION()
	void UpdateMaterial();



	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
