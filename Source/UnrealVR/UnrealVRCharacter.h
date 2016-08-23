// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "SpawnActor.h"
#include "ParticleDefinitions.h"
#include "EngineUtils.h"
#include "UnrealVRCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AUnrealVRCharacter : public ACharacter
{
	GENERATED_BODY()

	TMap<int32, UParticleSystemComponent*> particleSystemMap;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh, Replicated)
	class USkeletalMeshComponent* Mesh1P; //Mesh set in the Blueprint that inherits from this class

	UPROPERTY(VisibleAnywhere, Category = "Skeletal Mesh", Replicated)
	class USkeletalMeshComponent* avatar;

	//Particle system variable
	UPROPERTY(VisibleAnywhere, Category = "Particles", Replicated)
	class UParticleSystem* particleSystem;

	UPROPERTY(VisibleAnywhere, Category = "Particles", Replicated)
	UParticleSystemComponent* particleSystemInstance;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, Category = "SpawnObject")
	TSubclassOf<ASpawnActor> spawn;

	UPROPERTY(VisibleAnywhere, Category = "SpawnObject", Replicated)
	AActor *spawnInstance;

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

	int ID;
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

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/** Quit the game */
	void QuitGame();

	
	void spawnObject();
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_SpawnObject(FVector location);


	void spawnParticleEffect(AActor* start, AActor* end);
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_SpawnParticleEffect(AActor* start, AActor* end, int newID);


	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpawnParticleEffect(AActor* start, AActor* end, int newID);


	void despawnParticleEffect(int newID);
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_DespawnParticleEffect(int newID);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_DespawnParticleEffect(int newID);


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

	void SetID(int id);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;






	/*
	
			FUNCTIONS  AND VARIABLES TO CHECK FOR RTT TDT ETC.		
	
	*/

	int32 timer;
	FDateTime startTime, endTime;

	UPROPERTY(ReplicatedUsing = ReplicateSpawnTestArrival)
	ASpawnActor* spawnActorReplicateTest;

	UFUNCTION()
	void ReplicateSpawnTestArrival();
	UFUNCTION()
	void ReplicateSpawnTestStart();
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_Replication_SpawnTest(FVector location);


	void RTT_Test();
	UFUNCTION(Reliable, Server, WithValidation)
	void Server_RTT_Test();
	UFUNCTION(Client, Unreliable)
	void Client_RTT_Test();
};

