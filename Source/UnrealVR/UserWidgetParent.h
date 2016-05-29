// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "UserWidgetParent.generated.h"

/**
 * 
 */
UCLASS()
class UNREALVR_API UUserWidgetParent : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Code Variables")
	FString MyNewWidgetName;
	
	
};
