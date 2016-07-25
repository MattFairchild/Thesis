// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "UnrealVRGameMode.generated.h"

UCLASS(minimalapi)
class AUnrealVRGameMode : public AGameMode
{
	GENERATED_BODY()
private:
	int nextID;
public:
	AUnrealVRGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};



