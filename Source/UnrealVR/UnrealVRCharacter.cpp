// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UnrealVR.h"
#include "UnrealVRCharacter.h"
#include "UnrealVRProjectile.h"
#include "Shape.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "UnrealVRHUD.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AUnrealVRCharacter
AUnrealVRCharacter::AUnrealVRCharacter() : hit(ForceInit)
{
	PrimaryActorTick.bCanEverTick = true;

	timer = 0.0;

	inHand = nullptr;
	previous = nullptr;
	openMenu = nullptr;

	bReplicates = true;

	//set the root component to be the capsule component
	this->RootComponent = GetCapsuleComponent();

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(30.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = this->RootComponent;
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->AttachParent = FirstPersonCameraComponent;

	Mesh1P->RelativeRotation = FRotator(5.5f, -20.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> StaticHandsMeshOb(TEXT("SkeletalMesh'/Game/FirstPerson/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms'"));
	if (StaticHandsMeshOb.Succeeded())
	{
		Mesh1P->SetSkeletalMesh(StaticHandsMeshOb.Object);
	}

	//set the animation class of the hands. this is a blueprint animation class that is converted to Code before setting (via function)
	static ConstructorHelpers::FObjectFinder<UAnimBlueprintGeneratedClass> TmpHandsAnim(TEXT("AnimBlueprintGeneratedClass'/Game/FirstPerson/Animations/FirstPerson_AnimBP.FirstPerson_AnimBP_C'"));
	if (TmpHandsAnim.Succeeded())
	{
		Mesh1P->SetAnimInstanceClass(TmpHandsAnim.Object);
	}

	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;


	// Create the character that will be seen by any other particiapnts in the session (not seen by the owner)
	avatar = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh Component through Code"));
	avatar->AttachParent = FirstPersonCameraComponent;
	
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> StaticSkeletonMeshOb(TEXT("SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Robo.SK_CharM_Robo'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> StaticSkeletonMeshOb(TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (StaticSkeletonMeshOb.Succeeded())
	{
		avatar->SetSkeletalMesh(StaticSkeletonMeshOb.Object);
	}

	//set the animation class. this is a blueprint animation class that is converted to Code before setting (via function)
	static ConstructorHelpers::FObjectFinder<UAnimBlueprintGeneratedClass> TmpMeshAnim(TEXT("AnimBlueprintGeneratedClass'/Game/Mannequin/Animations/ThirdPerson_AnimBP.ThirdPerson_AnimBP_C'"));
	if (TmpMeshAnim.Succeeded())
	{
		avatar->SetAnimInstanceClass(TmpMeshAnim.Object);
	}
	
	avatar->SetOwnerNoSee(true); //set so that owner does not see his own mesh, should only see FPS hands
	avatar->RelativeLocation = FVector(50.0f, 0.0f, -157.0f);
	avatar->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);


	//Create the particle system component.
	ConstructorHelpers::FObjectFinder<UParticleSystem> ArbitraryParticleName(TEXT("ParticleSystem'/Game/ParticleSystem/ParticleSystems/P_Beam.P_Beam'"));
	particleSystem = CreateDefaultSubobject<UParticleSystem>(TEXT("ParticleSystemDefault"));

	if (ArbitraryParticleName.Succeeded()) 
	{
		particleSystem = ArbitraryParticleName.Object;
	}

	particleSystemInstance = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemInstance"));

	spawn = ASpawnActor::StaticClass();

	spawnWaiting = false;
	rttwaiting = false;
	spawniterations = 1;
	rttiterations = 1;
}

AUnrealVRCharacter::~AUnrealVRCharacter()
{
	if (spawnfile.is_open())
	{
		spawnfile.close();
	}

	if (rttfile.is_open())
	{
		rttfile.close();
	}
}

void AUnrealVRCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (!spawnfile.is_open())
	{
		spawnfile.open("C:/Users/mfa/Documents/Projects/Thesis/SPAWNLOG.txt", std::ofstream::out | std::ofstream::app);
	}

	if (!rttfile.is_open())
	{
		rttfile.open("C:/Users/mfa/Documents/Projects/Thesis/RTTLOG.txt", std::ofstream::out | std::ofstream::app);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input
void AUnrealVRCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up game play key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	InputComponent->BindAction("LeftClick", IE_Pressed, this, &AUnrealVRCharacter::leftClick);
	InputComponent->BindAction("RightClick", IE_Pressed, this, &AUnrealVRCharacter::ZoomIn);
	InputComponent->BindAction("RightClick", IE_Released, this, &AUnrealVRCharacter::ZoomOut);

	InputComponent->BindAction("MouseWheelUp", IE_Pressed, this, &AUnrealVRCharacter::mouseWheelUp);
	InputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &AUnrealVRCharacter::mouseWheelDown);
	InputComponent->BindAction("ChangeColor", IE_Pressed, this, &AUnrealVRCharacter::SwitchColor);
	InputComponent->BindAction("ESC", IE_Pressed, this, &AUnrealVRCharacter::QuitGame);

	InputComponent->BindAction("1", IE_Pressed, this, &AUnrealVRCharacter::ReplicateSpawnTestStart);
	InputComponent->BindAction("2", IE_Pressed, this, &AUnrealVRCharacter::RTT_Test);
	InputComponent->BindAction("8", IE_Pressed, this, &AUnrealVRCharacter::LogSpawn);
	InputComponent->BindAction("9", IE_Pressed, this, &AUnrealVRCharacter::LogRTT);


	InputComponent->BindAxis("MoveForward", this, &AUnrealVRCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AUnrealVRCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AUnrealVRCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AUnrealVRCharacter::LookUpAtRate);
}

void AUnrealVRCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AUnrealVRCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AUnrealVRCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AUnrealVRCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AUnrealVRCharacter::leftClick()
{
	ASpawnActor* actor = GetActorInFocus(true);

	if (actor && !inHand)
	{
		pickupObject(actor);
	}
	else if (inHand)
	{
		releaseObject();
	}
}

void AUnrealVRCharacter::QuitGame()
{
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}

void AUnrealVRCharacter::Tick(float DeltaTime)
{
	//Zoom in if ZoomIn button is down, zoom back out if it's not
	{
		if (bZoomingIn)
		{
			ZoomFactor += DeltaTime / 0.5f;         //Zoom in over half a second
		}
		else
		{
			ZoomFactor -= DeltaTime / 0.25f;        //Zoom out over a quarter of a second
		}
		ZoomFactor = FMath::Clamp<float>(ZoomFactor, 0.0f, 1.0f);
		//Blend our camera's FOV and our SpringArm's length based on ZoomFactor
		this->GetFirstPersonCameraComponent()->FieldOfView = FMath::Lerp<float>(90.0f, 60.0f, ZoomFactor);
	}


	if (inHand)
	{
		UCameraComponent* cam = this->GetFirstPersonCameraComponent();
		FVector location = cam->GetComponentLocation() + (cam->GetForwardVector() * hitDistance);
		
		positionObject(inHand, location);
	}
	//else
	//{
	//	higlightObject();
	//}
}

bool AUnrealVRCharacter::updateRaycastHit()
{
	auto player = UGameplayStatics::GetPlayerController(this, 0);

	FCollisionQueryParams CombatCollisionQuery(FName(TEXT("CombatTrace")), true, this);
	CombatCollisionQuery.bTraceAsyncScene = true;
	CombatCollisionQuery.bReturnPhysicalMaterial = true;

	FVector pos;
	FRotator rot;
	player->GetPlayerViewPoint(pos, rot);

	FVector direction = rot.Vector();
	FVector endTrace = pos + direction * 65000.0f;

	bool hasHit = GetWorld()->LineTraceSingleByChannel(hit, pos, endTrace, ECC_GameTraceChannel1, CombatCollisionQuery);


	return hasHit;
}

ASpawnActor* AUnrealVRCharacter::GetActorInFocus(bool onlyIfMovable)
{
	////if the player is holding something currently, then we don't need to check
	//if (inHand)
	//	return nullptr;

	//update raycast and if it has hit something, than proceed
	if (updateRaycastHit())
	{
		AActor* hitActor = hit.GetActor();
		bool moveable = false;
		hitDistance = hit.Distance;

		//RETURNING ONLY ASPAWNACTORS AT THE MOMENT, COULD BE ANY OTHER SCRIPT
		ASpawnActor* actor = Cast<ASpawnActor>(hitActor);
		if (!actor)
			return nullptr;

		//get the mobility of the object if it has a static mesh
		UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(actor->GetRootComponent());
		if (mesh)
		{
			FVector location = hit.GetActor()->GetActorLocation();

			moveable = (mesh->Mobility == EComponentMobility::Movable);

			if (onlyIfMovable)
			{
				//if I only want to get moveable meshes, then return according to the mobility of the object
				return (moveable ? actor : nullptr);
			}
			else
			{
				return actor;
			}
		}
	}

	//if no hit, then return nullptr
	return nullptr;
}


void AUnrealVRCharacter::higlightObject()
{
	inFocus = GetActorInFocus();

	/* We have 3 cases:
		1. there is no "previous" actor, so we just started off
		2. current actor we are looking at is the same as in last frame
		3. we are looking at a new actor (compared to last frame)
	*/

	//we set the outline effect through "SetRenderCustomDepth" if it is a moveable object we can pick up
	if (previous == nullptr)
	{
		//highlight(inFocus, true);
		previous = inFocus;
	}
	//if we are looking at the object inHand, we can disregard this (when picking up item we will always have in in the focus)
	else if (inFocus == previous)
	{
		return;
	}
	else
	{
		//if (previous)
		//{
		//	highlight(previous, false);
		//}

		//only if we have an actual object in Focus
		if (inFocus)
		{
			UStaticMeshComponent* currentmesh = Cast<UStaticMeshComponent>(inFocus->GetRootComponent());
			if (currentmesh)
			{
				if (currentmesh->Mobility == EComponentMobility::Movable)
					currentmesh->SetRenderCustomDepth(true);
			}

			previous = inFocus;
		}
	}
}

void AUnrealVRCharacter::ZoomIn()
{
	bZoomingIn = true;
}

void AUnrealVRCharacter::ZoomOut()
{
	bZoomingIn = false;
}

void AUnrealVRCharacter::mouseWheelUp()
{
	if (!inHand)
		return;

	hitDistance += 10.0f;
	FMath::Clamp<float>(hitDistance, 200.0f, 5000.0f);
}

void AUnrealVRCharacter::mouseWheelDown()
{
	if (!inHand)
		return;

	hitDistance -= 10.0f;
	FMath::Clamp<float>(hitDistance, 200.0f, 5000.0f);
}

bool AUnrealVRCharacter::tooCloseToObject()
{
	return (hitDistance < 200.0f);
}

void AUnrealVRCharacter::highlight(AActor* actor, bool highlightOn)
{
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(actor->GetRootComponent());
	if (mesh)
	{
		mesh->SetRenderCustomDepth(highlightOn);
	}
}

void AUnrealVRCharacter::spawnMenuWidget(FVector location)
{
	//if we already have a menu open, destroy it first
	if (openMenu)
		openMenu->Destroy();

	FRotator rotation = this->GetActorRotation();
	rotation.Yaw += 180.0f;
	openMenu = GetWorld()->SpawnActor<AActor>(widget, location, rotation);
}


void AUnrealVRCharacter::SetID(int id)
{
	this->ID = id;
}

/********************************************************************************/
/*								RPC calling functions							*/			
/********************************************************************************/

void AUnrealVRCharacter::spawnObject()
{
	if(updateRaycastHit())
		Server_SpawnObject(hit.Location);
}

void AUnrealVRCharacter::spawnParticleEffect(AActor* start, AActor* end)
{
	Server_SpawnParticleEffect(start, end, ID);
}

void AUnrealVRCharacter::despawnParticleEffect(int newID)
{
	Server_DespawnParticleEffect(newID);
}

void AUnrealVRCharacter::SwitchColor()
{
	if (inHand)
	{
		inHand->StartTimer();
		Server_ChangeInHandColor(inHand);
	}
	//if i press the changeColor button and I have a ASpawnActor in the reticle, then change color in spot	
	else
	{
		//do we have a SpawnAsctor under reticle that has not been pickup up yet?
		ASpawnActor* actor = GetActorInFocus();
		if (actor && !actor->IsInHand())
		{
			actor->StartTimer();
			Server_ChangeInHandColor(actor);
		}
	}
}

void AUnrealVRCharacter::releaseObject()
{
	//highlight(inHand, false);

	Server_ReleaseObject(inHand);
	despawnParticleEffect(ID);

	inHand = nullptr;
}

void AUnrealVRCharacter::pickupObject(ASpawnActor* actor)
{
	//dont be able to pick up object if we are too close -> weird behaviour
	if (tooCloseToObject() || actor->IsInHand())
	{
		return;
	}

	inHand = actor;
	Server_PickupObject(actor);
	spawnParticleEffect(this, inHand);

	//turn highlight on actor off
	//highlight(actor, false);
}


void AUnrealVRCharacter::positionObject(AActor* actor, FVector location)
{
	actor->SetActorLocation(location); //
	Server_PositionObject(actor, location);
}

/********************************************************************************/
/*								RPC  FUNCTIONS									*/
/********************************************************************************/



/*								SPAWN OBJECT									*/
void AUnrealVRCharacter::Server_SpawnObject_Implementation(FVector location)
{
	ASpawnActor* actor = GetWorld()->SpawnActor <ASpawnActor>(spawn, location, GetActorRotation());
	actor->SetRandomColor();
}

bool AUnrealVRCharacter::Server_SpawnObject_Validate(FVector location)
{
	return true;
}



/*								SPAWN PARTICLE EFFECT									*/
void AUnrealVRCharacter::Server_SpawnParticleEffect_Implementation(AActor* start, AActor* end, int newID)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Particle Effect spawned"));

	Multicast_SpawnParticleEffect(start, end, newID);
}

bool AUnrealVRCharacter::Server_SpawnParticleEffect_Validate(AActor* start, AActor* end, int newID)
{
	return true;
}

void AUnrealVRCharacter::Multicast_SpawnParticleEffect_Implementation(AActor* start, AActor* end, int newID)
{
	particleSystemInstance = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleSystem, start->GetActorTransform(), false);
	particleSystemInstance->SetActorParameter("BeamSource", start);
	particleSystemInstance->SetActorParameter("BeamTarget", end);
	particleSystemInstance->ActivateSystem();

	particleSystemMap.Add(newID, particleSystemInstance);
}


/*								DESPAWN PARTICLE EFFECT									*/
void AUnrealVRCharacter::Server_DespawnParticleEffect_Implementation(int newID)
{
	Multicast_DespawnParticleEffect(newID);
}

bool AUnrealVRCharacter::Server_DespawnParticleEffect_Validate(int newID)
{
	return true;
}

void AUnrealVRCharacter::Multicast_DespawnParticleEffect_Implementation(int newID)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Despawning Particle Effect"));

	particleSystemMap[newID]->Deactivate();
	particleSystemMap.Remove(newID);
}


/*								CHANGE COLOR									*/
void AUnrealVRCharacter::Server_ChangeInHandColor_Implementation(ASpawnActor* actor)
{
	actor->SwitchColors();
}

bool AUnrealVRCharacter::Server_ChangeInHandColor_Validate(ASpawnActor* actor)
{
	return true;
}



/*								PICKUP OBJECT									*/
void AUnrealVRCharacter::Server_PickupObject_Implementation(ASpawnActor* actor)
{
	actor->SetIsInHand(true);
	actor->GetMesh()->SetSimulatePhysics(false);
}

bool AUnrealVRCharacter::Server_PickupObject_Validate(ASpawnActor* actor)
{
	return true;
}



/*								RELEASE OBJECT									*/
void AUnrealVRCharacter::Server_ReleaseObject_Implementation(ASpawnActor* actor)
{
	actor->SetIsInHand(false);
	actor->GetMesh()->SetSimulatePhysics(true);
}

bool AUnrealVRCharacter::Server_ReleaseObject_Validate(ASpawnActor* actor)
{
	return true;
}



/*								POSITION OBJECT									*/
void AUnrealVRCharacter::Server_PositionObject_Implementation(AActor* actor, FVector location)
{
	actor->SetActorLocation(location);
}

bool AUnrealVRCharacter::Server_PositionObject_Validate(AActor* actor, FVector location)
{
	return true;
}



/********************************************************************************/
/*								RPC  FUNCTIONS									*/
/********************************************************************************/

void AUnrealVRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Here we list the variables we want to replicate + a condition if wanted DOREPLIFETIME(ATestPlayerCharacter, Health);

	DOREPLIFETIME(AUnrealVRCharacter, Mesh1P);
	//DOREPLIFETIME(AUnrealVRCharacter, bladeChar);
	DOREPLIFETIME(AUnrealVRCharacter, spawnInstance);
	DOREPLIFETIME(AUnrealVRCharacter, spawnActorReplicateTest);
	DOREPLIFETIME(AUnrealVRCharacter, spawnActorReplicateTestWithLog);
}










//////////////////////////////////////////////////////

/*

			FUNCTIONS TO CHECK FOR RTT TDT ETC.

*/

//////////////////////////////////////////////////////


void AUnrealVRCharacter::RTT_Test()
{
	startTime = FDateTime::UtcNow();

	if (!rttwaiting)
	{
		rttwaiting = true;
		Server_RTT_Test();
	}
}


void AUnrealVRCharacter::Server_RTT_Test_Implementation(bool log)
{
	//if i want to log, then i must take the iterations into account
	if (log && rttiterations > 0)
	{
		Client_RTT_Test(true);
	}
	//iterations not relevant if no logging
	else if (!log)
	{
		Client_RTT_Test(false);
	}
}


bool AUnrealVRCharacter::Server_RTT_Test_Validate(bool log)
{
	return true;
}


void AUnrealVRCharacter::Client_RTT_Test_Implementation(bool log)
{
	endTime = FDateTime::UtcNow();
	timer = endTime.GetMillisecond() - startTime.GetMillisecond();
	rttwaiting = false;

	FString str = TEXT("");
	str.AppendInt(timer);
	str.Append(TEXT(" ms, RTT TEST"));

	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, str);	
	
	if (log && rttfile.is_open())
	{
		rttfile << timer << std::endl;
		rttiterations--;

		if (rttiterations > 0)
		{
			startTime = FDateTime::UtcNow();

			if (!rttwaiting)
			{
				rttwaiting = true;
				Server_RTT_Test(true);
			}
		}
	}
}





void AUnrealVRCharacter::ReplicateSpawnTestStart()
{
	if (updateRaycastHit())
	{
		startTime = FDateTime::UtcNow();
		Server_Replication_SpawnTest(hit.Location);
	}
}

void AUnrealVRCharacter::ReplicateSpawnTestArrival()
{
	endTime = FDateTime::UtcNow();
	timer = endTime.GetMillisecond() - startTime.GetMillisecond();

	FString str = TEXT("");
	str.AppendInt(timer);
	str.Append(TEXT(" ms, replication test on spawning actor"));

	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Emerald, str);
}

void AUnrealVRCharacter::Server_Replication_SpawnTest_Implementation(FVector location)
{
	ASpawnActor* actor = GetWorld()->SpawnActor <ASpawnActor>(spawn, location, GetActorRotation());
	actor->SetRandomColor();

	spawnActorReplicateTest = actor;
}


bool AUnrealVRCharacter::Server_Replication_SpawnTest_Validate(FVector location)
{
	return true;
}



void AUnrealVRCharacter::ReplicateSpawnTestStartWithLog()
{
	if (updateRaycastHit())
	{
		startTime = FDateTime::UtcNow();
		Server_Replication_SpawnTestWithLog(hit.Location); //RANDOM LOCATION BETTER ?
	}
}
void AUnrealVRCharacter::ReplicateSpawnTestArrivalWithLog()
{
	endTime = FDateTime::UtcNow();
	timer = endTime.GetMillisecond() - startTime.GetMillisecond();

	if (spawnfile.is_open())
	{
		spawnfile << timer << std::endl;
	}

	FString str = TEXT("");
	str.AppendInt(timer);
	str.Append(TEXT(" ms, replication test on spawning actor"));

	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Emerald, str);

	// recursively write
	if (spawniterations > 0)
	{
		spawniterations--;
		ReplicateSpawnTestStartWithLog();
	}
}
void AUnrealVRCharacter::Server_Replication_SpawnTestWithLog_Implementation(FVector location)
{
	ASpawnActor* actor = GetWorld()->SpawnActor <ASpawnActor>(spawn, location, GetActorRotation());
	actor->SetRandomColor();

	spawnActorReplicateTestWithLog = actor;
}
bool AUnrealVRCharacter::Server_Replication_SpawnTestWithLog_Validate(FVector location)
{
	return true;
}







void AUnrealVRCharacter::LogRTT()
{
	rttiterations = 100;
	Server_RTT_Test(true);
}


void AUnrealVRCharacter::LogSpawn()
{
	spawniterations = 100;
	ReplicateSpawnTestStartWithLog();
}