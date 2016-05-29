// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UserWidgetParent.h"
#include "UserWidgetActorCode.generated.h"

UCLASS()
class UNREALVR_API AUserWidgetActorCode : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUserWidgetActorCode();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidgetParent> widget;
	
};
