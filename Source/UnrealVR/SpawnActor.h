// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include <fstream>
#include "SpawnActor.generated.h"

UCLASS()
class UNREALVR_API ASpawnActor : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	class UStaticMeshComponent* mesh;

	UPROPERTY(Replicated)
	bool isInHand;

	UPROPERTY(VisibleAnywhere, Category = "Color")
	TArray<UMaterialInterface*> mats;

	UPROPERTY(ReplicatedUsing = UpdateMaterial)
	int currentMat;

	UFUNCTION()
	void UpdateMaterial();

	UPROPERTY(Replicated)
	bool waiting;
	int32 timer, serverTimer;
	FDateTime startTime, endTime, serverStartTime, serverEndTime;

	std::ofstream colorchangefile, serverTimeVariableChangeFile;
	std::string filename, serverFileName;
public:
	int32 rounds;

public:	
	// Sets default values for this actor's properties
	ASpawnActor();
	~ASpawnActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UStaticMeshComponent* GetMesh() const;
	bool IsInHand() const;

	void SwitchColors();

	void StartTimer();

	void SetRandomColor();

	void SetIsInHand(bool newVal);

	int32 getTimePassed(FDateTime start, FDateTime end);

	UFUNCTION(Reliable, Server, WithValidation)
	void Server_SwitchColors();
	UFUNCTION(Client, Reliable)
	void Client_LogServerTime(int32 time);

	//function to set all the replicated variables into
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
