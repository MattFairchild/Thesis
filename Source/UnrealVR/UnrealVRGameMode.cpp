// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UnrealVR.h"
#include "UnrealVRGameMode.h"
#include "UnrealVRHUD.h"
#include "UnrealVRCharacter.h"

AUnrealVRGameMode::AUnrealVRGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Script/UnrealVR.UnrealVRCharacter"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/MyUnrealVRCharacter_BP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AUnrealVRHUD::StaticClass();

	nextID = 0;
}

void AUnrealVRGameMode::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);

	AUnrealVRCharacter* character = Cast<AUnrealVRCharacter>(newPlayer->GetCharacter());
	if (character)
	{
		character->SetID(nextID++);

		FString str = TEXT("New Player registered, num: ");
		str.AppendInt(nextID);

		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, str);
	}
}