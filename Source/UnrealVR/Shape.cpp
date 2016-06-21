// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealVR.h"
#include "Shape.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

// Sets default values
AShape::AShape(const class FObjectInitializer &PCIP) : Super(PCIP)
{
	SetMobility(EComponentMobility::Movable);

	bReplicates = true;
	bStaticMeshReplicateMovement = true;
	bReplicateMovement = true;
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

void AShape::switchColors_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is called on the server"));
	currentMat = (currentMat + 1) % mats.Num();
	
	//needs to be called manually,since in C++ the server does not call the RepNotify function automatically
	UpdateMaterial();
}

bool AShape::switchColors_Validate()
{
	return true;
}


void AShape::UpdateMaterial()
{
	this->GetStaticMeshComponent()->SetMaterial(0, mats[currentMat]);
}


void AShape::setShapePosition_Implementation(FVector location)
{
	this->SetActorLocation(location);
}

bool AShape::setShapePosition_Validate(FVector location)
{
	return true;
}

void AShape::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Here we list the variables we want to replicate + a condition if wanted DOREPLIFETIME(ATestPlayerCharacter, Health);

	DOREPLIFETIME(AShape, currentMat);
}