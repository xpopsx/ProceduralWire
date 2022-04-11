// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components.h"
#include "ProceduralMeshComponent.h" 
#include "WireActor.generated.h"

UCLASS()
class WIRES_API AWireActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWireActor();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* RootSceneComponent; 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* WireStartPoint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* WireEndPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "3", ClampMax = "100", UIMin = "3", UIMax = "100"))
		int32 WireCellCount = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		int32 WireGravityScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "3", UIMin = "3"))
		int32 WireCylinderSectorCount = 6;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", UIMin = "1"))
		float WireCylinderRadius = 5;

	virtual bool ShouldTickIfViewportsOnly() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent* MeshComponent = nullptr;
	
	virtual void GenerateCylinderMesh();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	virtual void UpdateWireWorldLocation();

	TArray<FVector> WirePointLocation;

	bool bMeshGenerated;
	bool bNeedMeshUpdate;

	int32 WireBufferCylinderSectorCount = 6;
};
