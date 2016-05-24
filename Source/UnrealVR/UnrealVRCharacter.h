// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "UnrealVRCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AUnrealVRCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY()
	AActor *inHand;

	UPROPERTY()
	AActor *inFocus;

	UPROPERTY()
	AActor *previous;
	
	bool bZoomingIn;
	float hitDistance;
	float ZoomFactor;
public:
	AUnrealVRCharacter();

	virtual void BeginPlay();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;


	UPROPERTY(EditAnywhere, Category = "ShapeSpawning")
	TSubclassOf<class AShape> shapeBP;

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	void leftClick();
	void ZoomIn();
	void ZoomOut();
	void mouseWheelUp();
	void mouseWheelDown();
	void changeInHandColor();

	AActor* currentlyInFocus(bool onlyIfMovable = false);

	//check for objects in focus and set the highlight accordingly
	void higlightObject();

	//turn the highlight of a specific actor on or off
	void highlight(AActor* actor, bool highlightOn);

	//pick the passed object up
	void pickupObject(AActor* actor);

	//drop the object in the players hand, if he has one
	void releaseObject();

	//dont be able to pick up object if we are too close -> weird behaviour
	bool tooCloseToObject();

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	//ticks every frame
	void Tick(float DeltaTime) override;
};

