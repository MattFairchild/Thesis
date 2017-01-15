// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealVR.h"
#include "SpawnActor.h"
#include <string>


// Sets default values
ASpawnActor::ASpawnActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bReplicateMovement = true;
	bAlwaysRelevant = true;

	isInHand = false;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnableMesh)"));
	this->RootComponent = mesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshOb(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_QuadPyramid.Shape_QuadPyramid'"));
	if (StaticMeshOb.Succeeded())
	{
		mesh->SetStaticMesh(StaticMeshOb.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> mat1(TEXT("MaterialInstanceConstant'/Game/Materials/ColorMaterial_Red.ColorMaterial_Red'"));
	if (mat1.Succeeded())
	{
		mats.Add(mat1.Object);
	}


	static ConstructorHelpers::FObjectFinder<UMaterialInstance> mat2(TEXT("MaterialInstanceConstant'/Game/Materials/ColorMaterial_Blue.ColorMaterial_Blue'"));
	if (mat2.Succeeded())
	{
		mats.Add(mat2.Object);
	}


	static ConstructorHelpers::FObjectFinder<UMaterialInstance> mat3(TEXT("MaterialInstanceConstant'/Game/Materials/ColorMaterial_Yellow.ColorMaterial_Yellow'"));
	if (mat3.Succeeded())
	{
		mats.Add(mat3.Object);
	}

	rounds = 1;
	currentMat = 1;
	waiting = false;

	FString pcName = FPlatformMisc::GameDir();
	std::string pathPrelim = TCHAR_TO_UTF8(*pcName);
	filename = pathPrelim + "VariableChange.csv";

	UpdateMaterial();
}

ASpawnActor::~ASpawnActor()
{
	if (colorchangefile.is_open())
	{
		colorchangefile.close();
	}
}

// Called when the game starts or when spawned
void ASpawnActor::BeginPlay()
{
	Super::BeginPlay();
	waiting = false;
}


void ASpawnActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Here we list the variables we want to replicate + a condition if wanted DOREPLIFETIME(ATestPlayerCharacter, Health);

	DOREPLIFETIME(ASpawnActor, isInHand);
	DOREPLIFETIME(ASpawnActor, currentMat);
}


UStaticMeshComponent* ASpawnActor::GetMesh() const
{
	return mesh;
}

void ASpawnActor::UpdateMaterial()
{
	mesh->SetMaterial(0, mats[currentMat]);

	if (GetNetMode() == ENetMode::NM_Client && waiting)
	{
		//get the time of arrival here and print IF on client
		waiting = false;
		endTime = FDateTime::UtcNow();

		//make sure time is not negative, but rather accurate also when crossing ms to seconds boundaries
		{
			int32 result = endTime.GetMillisecond() - startTime.GetMillisecond();
			int32 startSeconds = startTime.GetSecond(), endSeconds = endTime.GetSecond();

			if (endSeconds > startSeconds)
			{
				int32 diff = endSeconds - startSeconds;
				timer = result + (1000 * diff);
			}
			else
			{
				timer = result;
			}
		}



		FString str = TEXT("");
		str.AppendInt(timer);
		str.Append(TEXT(" ms, replication test on variable change"));

		//write the time into the file
		{
			if (!colorchangefile.is_open())
			{
				colorchangefile.open(filename, std::ofstream::out | std::ofstream::app);
			}

			colorchangefile << FGenericPlatformProcess::ComputerName() << ";" << timer << std::endl;

			if (colorchangefile.is_open())
			{
				colorchangefile.close();
			}
		}


		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Magenta, str);

		rounds--;
		if (rounds > 0)
		{
			StartTimer();
			Server_SwitchColors();
		}
	}
}

void ASpawnActor::SwitchColors()
{
	currentMat = (currentMat + 1) % mats.Num();

	//needs to be called manually,since in C++ the server does not call the RepNotify function automatically. CLIENTS DO, but not the server itself
	if(GetNetMode() < ENetMode::NM_Client)
	{
		UpdateMaterial();
	}
}

void ASpawnActor::SetRandomColor()
{
	currentMat = rand() % mats.Num();

	//needs to be called manually,since in C++ the server does not call the RepNotify function automatically
	UpdateMaterial();
}

void ASpawnActor::StartTimer()
{
	waiting = true;
	startTime = FDateTime::UtcNow();
}

void ASpawnActor::SetIsInHand(bool newVal)
{
	isInHand = newVal;
}

bool ASpawnActor::IsInHand() const
{
	return isInHand;
}

void ASpawnActor::Server_SwitchColors_Implementation()
{
	currentMat = (currentMat + 1) % mats.Num();

	//needs to be called manually,since in C++ the server does not call the RepNotify function automatically. CLIENTS DO, but not the server itself
	UpdateMaterial();
}

bool ASpawnActor::Server_SwitchColors_Validate()
{
	return true;
}