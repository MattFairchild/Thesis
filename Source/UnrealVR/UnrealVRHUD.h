// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "UnrealVRHUD.generated.h"

UCLASS()
class AUnrealVRHUD : public AHUD
{
	GENERATED_BODY()

public:
	AUnrealVRHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	UCanvas* getCanvas() const;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

