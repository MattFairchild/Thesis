// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealVR.h"
#include "Shape.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

// Sets default values
AShape::AShape(const class FObjectInitializer &PCIP) : Super(PCIP)
{
	SetMobility(EComponentMobility::Movable);	
}

void AShape::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//have physics starting
	UPrimitiveComponent* comp = Cast<UPrimitiveComponent>(this->GetRootComponent());
	if (comp)
	{
		comp->SetSimulatePhysics(true);
	}

	currentMat = 0;
	//Set material
	if (mats.Num() > 0)
	{
		this->GetStaticMeshComponent()->SetMaterial(0, mats[currentMat]);
	}

}

void AShape::switchColors()
{
	currentMat = (currentMat + 1) % mats.Num();
	this->GetStaticMeshComponent()->SetMaterial(0, mats[currentMat]);
}