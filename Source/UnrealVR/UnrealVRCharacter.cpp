// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UnrealVR.h"
#include "UnrealVRCharacter.h"
#include "UnrealVRProjectile.h"
#include "Shape.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include <Private/GameplayDebugger/AIModuleBasicGameplayDebuggerObject.h>
#include "UnrealVRHUD.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AUnrealVRCharacter

AUnrealVRCharacter::AUnrealVRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	pickup = nullptr;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AUnrealVRCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUnrealVRCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	InputComponent->BindAction("LeftClick", IE_Pressed, this, &AUnrealVRCharacter::leftClick);
	InputComponent->BindAction("RightClick", IE_Pressed, this, &AUnrealVRCharacter::rightClick);

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
	auto player = UGameplayStatics::GetPlayerController(this, 0);
	int x, y;
	player->GetViewportSize(x, y);

	UE_LOG(LogFPChar, Warning, TEXT("GetViewport size X. %d, Y: %d"), x, y);

	FHitResult hit;
	FCollisionQueryParams CombatCollisionQuery(FName(TEXT("CombatTrace")), true, NULL);

	UCameraComponent* camera = this->GetFirstPersonCameraComponent();

	FVector loc = camera->GetComponentLocation();

	FVector2D center(x * 0.5f, y * 0.5f);
	bool hasHit = player->GetHitResultAtScreenPosition(center, ECollisionChannel::ECC_Visibility, CombatCollisionQuery, hit);

	if(hasHit)
	{
		GetWorld()->SpawnActor<AShape>(shapeBP, hit.Location, this->GetActorRotation());
		
		AActor* actor = hit.GetActor();
		UPrimitiveComponent* comp = Cast<UPrimitiveComponent>(actor->GetRootComponent());
		if (comp)
		{
			comp->SetSimulatePhysics(false);		
			hitDistance = hit.Distance;
			pickup = actor;
		}

	}

	DrawDebugLine(camera->GetWorld(), loc, hit.Location, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f, 0, 2.0f);

	FVector location = hit.GetActor()->GetActorLocation();
	UE_LOG(LogFPChar, Warning, TEXT("Hit Character wit name \"%s\" at loca is %d, %d, %d"), (hasHit ? *hit.GetComponent()->GetName() : TEXT("NO hit")), location.X, location.Y, location.Z);
}

void AUnrealVRCharacter::rightClick()
{
	if (pickup)
	{
		pickup->DetachRootComponentFromParent();
		UPrimitiveComponent* comp = Cast<UPrimitiveComponent>(pickup->GetRootComponent());
		comp->SetSimulatePhysics(true);
		pickup = nullptr;
	}
}

void AUnrealVRCharacter::Tick(float DeltaTime)
{
	if (pickup)
	{
		UCameraComponent* cam = this->GetFirstPersonCameraComponent();
		FVector location = cam->GetComponentLocation() + (cam->GetForwardVector() * hitDistance);
		pickup->SetActorRotation(cam->GetComponentRotation());
		pickup->SetActorLocation(location);
	}
}