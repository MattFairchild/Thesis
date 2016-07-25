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
}

void AUnrealVRGameMode::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, TEXT("Testing"));
}