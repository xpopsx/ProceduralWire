// Fill out your copyright notice in the Description page of Project Settings.


#include "WireActor.h"
#include "DrawDebugHelpers.h"

// Sets default values
AWireActor::AWireActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	WireStartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StartPoint"));
	WireEndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndPoint"));

	RootSceneComponent->SetupAttachment(RootComponent);
	RootSceneComponent->bVisualizeComponent = true;
	WireStartPoint->SetupAttachment(RootSceneComponent);
	WireEndPoint->SetupAttachment(RootSceneComponent);
	TArray<USceneComponent*> SceneArray = { WireStartPoint, WireEndPoint };
	for (int8 i = 0; i < SceneArray.Num(); i++)
	{
		SceneArray[i]->bVisualizeComponent = true;
		SceneArray[i]->SetRelativeLocation(FVector(i * 30, 0.0f, 0.0f));
	}

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("WireMeshComponent");
	MeshComponent->bUseAsyncCooking = true;
	MeshComponent->SetupAttachment(RootSceneComponent);
	//GenerateCylinderMesh();

}

bool AWireActor::ShouldTickIfViewportsOnly() const
{
	return true;
}

// Called when the game starts or when spawned
void AWireActor::BeginPlay()
{
	Super::BeginPlay();
}

void AWireActor::GenerateCylinderMesh()
{
	if (MeshComponent && !bMeshGenerated) 
	{
			bMeshGenerated = true;
			
			TArray<FVector> Vertices;
			TArray<int32> Triangles;
			TArray<FVector> normals;
			TArray<FProcMeshTangent> tangents;
			TArray<FLinearColor> vertexColors;
			TArray<FVector2D> UV0;

			float CylinderRadius = 10;
			int CylinderFaceCount = WireCylinderSectorCount;
			TArray<FVector2D> Face2dVerticesArray;

			for (int CellIndex = 0; CellIndex < WireCellCount; CellIndex++)
			{
				FVector RelativeStartWireVector = ((WireStartPoint->GetComponentLocation() - WireEndPoint->GetComponentLocation()) / WireCellCount) * CellIndex
					+ WireEndPoint->GetComponentLocation()
					- WirePointLocation[CellIndex]
					+ ((WireStartPoint->GetRelativeLocation() - WireEndPoint->GetRelativeLocation()) / WireCellCount) * CellIndex;

				int EndCellIndex = (CellIndex + 1) > WireCellCount ? CellIndex : (CellIndex + 1);
				FVector RelativeEndWireVector = ((WireStartPoint->GetComponentLocation() - WireEndPoint->GetComponentLocation()) / WireCellCount) * EndCellIndex
					+ WireEndPoint->GetComponentLocation()
					- WirePointLocation[EndCellIndex]
					+ ((WireStartPoint->GetRelativeLocation() - WireEndPoint->GetRelativeLocation()) / WireCellCount) * EndCellIndex;

				float DeltaAngle = (WireEndPoint->GetRelativeLocation().Y / WireEndPoint->GetRelativeLocation().X);
				DeltaAngle = FMath::Atan(DeltaAngle);

				for (int FaceIndex = 0; FaceIndex <= CylinderFaceCount; FaceIndex++)
				{
					// find face points;
					float PieceFaceAngle = 360.0f / CylinderFaceCount * FaceIndex;
					PieceFaceAngle = FMath::DegreesToRadians(PieceFaceAngle);
					float FacePointZ = CylinderRadius * FMath::Cos(PieceFaceAngle);
					float FacePointY = CylinderRadius * FMath::Sin(PieceFaceAngle);
					Face2dVerticesArray.Add(FVector2D(FacePointY, FacePointZ));
				}

				//Make SingleCylinre

				for (FVector2D Vector2d : Face2dVerticesArray)
				{
					FVector Vector1 = FVector(0, Vector2d.Y, Vector2d.X) - RelativeStartWireVector;
					Vertices.Add(Vector1);
					FVector Vector2 = FVector(0, Vector2d.Y, Vector2d.X) - RelativeEndWireVector;
					Vertices.Add(Vector2);
				}
				for (int i = 0; i < Vertices.Num(); i++)
				{
					if (i % 2 == 0)
					{
						if (i + 3 < Vertices.Num())
						{
							Triangles.Add(i);
							Triangles.Add(i + 1);
							Triangles.Add(i + 2);
							Triangles.Add(i + 1);
							Triangles.Add(i + 3);
							Triangles.Add(i + 2);

							UV0.Add(FVector2D(0, 0));
							UV0.Add(FVector2D(10, 0));
							UV0.Add(FVector2D(0, 10));
							UV0.Add(FVector2D(10, 10));
						}
					}
				}

				for (int i = 0; i < Vertices.Num(); i++)
				{
					normals.Add(FVector(1, 0, 0));
					tangents.Add(FProcMeshTangent(0, 1, 0));
					vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
				}

				MeshComponent->CreateMeshSection_LinearColor(0, Vertices, Triangles, normals, UV0, vertexColors, tangents, true);

				// Enable collision data
				MeshComponent->ContainsPhysicsTriMeshData(false);
			}
	}
}

// Called every frame
void AWireActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if WITH_EDITOR
	UpdateWireWorldLocation();
	if (WireBufferCylinderSectorCount != WireCylinderSectorCount) 
	{
		WireBufferCylinderSectorCount = WireCylinderSectorCount;
		bMeshGenerated = false;
		GenerateCylinderMesh();
	}
	//MeshComponent->UpdateMeshSection()

#endif
}

void AWireActor::UpdateWireWorldLocation()
{
	DrawDebugLine(this->GetWorld(), WireStartPoint->GetComponentLocation(), WireEndPoint->GetComponentLocation(), FColor::Red, false, GetWorld()->DeltaTimeSeconds * 2, 0, 0.5f);
	
	for (int32 item = 0; item < WirePointLocation.Num(); item++)
	{
		if (item > WireCellCount) 
		{
			if (WirePointLocation.IsValidIndex(item)) WirePointLocation.RemoveAt(item);
		}
	}

	//

	for (int32 i = 0; i <= WireCellCount; i++)
	{
		if (!WirePointLocation.IsValidIndex(i)) 
		{
			WirePointLocation.Add(FVector::ZeroVector);
		}
		FHitResult HitResult;
		WirePointLocation[i] = ((WireStartPoint->GetComponentLocation() - WireEndPoint->GetComponentLocation()) / WireCellCount) * i + WireEndPoint->GetComponentLocation();

		// Calculate gravity function for wire y = ax^2 + bx +c, b = 0
		int32 GravityScale = WireGravityScale;

		float MinGravityX = GravityScale * 2.0f / WireCellCount;
		float GravityX = (i - (WireCellCount / 2.0f)) * MinGravityX;
		float GravityY = - i;
		float GravityDefenition = GravityX * GravityX - GravityScale * GravityScale;

		FVector EndTracePoint = FVector(WirePointLocation[i].X, WirePointLocation[i].Y, WirePointLocation[i].Z + GravityDefenition);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		GetWorld()->LineTraceSingleByChannel(HitResult, WirePointLocation[i], EndTracePoint, ECollisionChannel::ECC_Visibility, QueryParams);

		if (HitResult.bBlockingHit)
		{
			DrawDebugLine(this->GetWorld(), WirePointLocation[i], HitResult.Location, FColor::Green, false, GetWorld()->DeltaTimeSeconds * 2, 0, 0.5f);
			DrawDebugPoint(this->GetWorld(), HitResult.Location, 10.0f, FColor::Red, false, GetWorld()->DeltaTimeSeconds * 2, 0);
			WirePointLocation[i] = HitResult.Location;
		}
		else
		{
			DrawDebugLine(this->GetWorld(), WirePointLocation[i], EndTracePoint, FColor::Green, false, GetWorld()->DeltaTimeSeconds * 2, 0, 0.5f);
			WirePointLocation[i] = EndTracePoint;
		}
		//DrawDebugCircle(this->GetWorld(), )
	}
	for (int32 j = 0; j < (WirePointLocation.Num() - 1); j++)
	{
		 DrawDebugLine(this->GetWorld(), WirePointLocation[j], WirePointLocation[j + 1], FColor::Magenta, false, GetWorld()->DeltaTimeSeconds * 2, 0, 0.5f);
	}

}

