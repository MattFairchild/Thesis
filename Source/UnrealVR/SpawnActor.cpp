// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealVR.h"
#include "SpawnActor.h"


// Sets default values
ASpawnActor::ASpawnActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bReplicateMovement = true;
	bAlwaysRelevant = true;

	isInHand = false;
	currentMat = 0;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnableMesh)"));	
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshOb(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_QuadPyramid.Shape_QuadPyramid'"));
	if (StaticMeshOb.Succeeded())
	{
		mesh->SetStaticMesh(StaticMeshOb.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> mat1(TEXT("MaterialInstanceConstant'/Game/Materials/ColorMaterial_Inst.ColorMaterial_Inst'"));
	if (mat1.Succeeded())
	{
		mats.Add(mat1.Object);
	}


	static ConstructorHelpers::FObjectFinder<UMaterialInstance> mat2(TEXT("MaterialInstanceConstant'/Game/Materials/ColorMaterial_Inst2.ColorMaterial_Inst2'"));
	if (mat2.Succeeded())
	{
		mats.Add(mat2.Object);
	}

	UpdateMaterial();
}

// Called when the game starts or when spawned
void ASpawnActor::BeginPlay()
{
	Super::BeginPlay();
}


void ASpawnActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Here we list the variables we want to replicate + a condition if wanted DOREPLIFETIME(ATestPlayerCharacter, Health);

	DOREPLIFETIME(ASpawnActor, isInHand);
	DOREPLIFETIME(ASpawnActor, currentMat);
}


UStaticMeshComponent* ASpawnActor::getMesh() const
{
	return mesh;
}

void ASpawnActor::UpdateMaterial()
{
	mesh->SetMaterial(0, mats[currentMat]);
}

void ASpawnActor::SwitchColors()
{
	currentMat = (currentMat + 1) % mats.Num();

	//needs to be called manually,since in C++ the server does not call the RepNotify function automatically
	UpdateMaterial();
}

void ASpawnActor::setIsInHand(bool newVal)
{
	isInHand = newVal;
}