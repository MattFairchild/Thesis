// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "RPCManager.h"
#include "SpawnActor.h"
#include "UnrealVRCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AUnrealVRCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh, Replicated)
	class USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleAnywhere, Category = "Skeletal Mesh", Replicated)
	class USkeletalMeshComponent* bladeChar;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY()
	ASpawnActor *inHand;

	UPROPERTY()
	AActor *inFocus;

	UPROPERTY()
	AActor *previous;
	
	bool bZoomingIn;
	float hitDistance;
	float ZoomFactor;
	FHitResult hit;

	AActor* openMenu;
public:
	AUnrealVRCharacter();

	virtual void BeginPlay();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, Category = "Widget Spawning")
	TSubclassOf<class AActor> widget;

	UPROPERTY(EditAnywhere, Category = "Widget Spawning")
	TSubclassOf<class AActor> spawnThing;

	UPROPERTY(VisibleAnywhere)
	ARPCManager* rpc;

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);


	void spawnObject();
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_SpawnObject(FVector location);

	void SwitchColor();
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_ChangeInHandColor(ASpawnActor* actor);

	//pick the passed object up
	void pickupObject(ASpawnActor* actor);
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_PickupObject(ASpawnActor* actor);

	//drop the object in the players hand, if he has one
	void releaseObject();
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_ReleaseObject(ASpawnActor* actor);

	//drop the object in the players hand, if he has one
	void positionObject(AActor* actor, FVector location);
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_PositionObject(AActor* actor, FVector location);


	void spawnMenuWidget(FVector location);


	/*Misc Input bindings*/
	void leftClick();
	void ZoomIn();
	void ZoomOut();
	void mouseWheelUp();
	void mouseWheelDown();

	ASpawnActor* GetActorInFocus(bool onlyIfMovable = false);

	bool updateRaycastHit();

	//check for objects in focus and set the highlight accordingly
	void higlightObject();

	//turn the highlight of a specific actor on or off
	void highlight(AActor* actor, bool highlightOn);

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


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};

