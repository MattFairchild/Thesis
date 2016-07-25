// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UnrealVR.h"
#include "UnrealVRGameMode.h"
#include "UnrealVRHUD.h"
#include "UnrealVRCharacter.h"

AUnrealVRGameMode::AUnrealVRGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AUnrealVRHUD::StaticClass();

	nextID = 0;
}

void AUnrealVRGameMode::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);

	AUnrealVRCharacter* character = Cast<AUnrealVRCharacter>(newPlayer);
	if (character)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, TEXT("New Player registered"));
		character->SetID(nextID++);
	}
}