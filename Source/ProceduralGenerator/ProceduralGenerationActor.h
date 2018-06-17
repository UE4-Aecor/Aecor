// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralGenerationActor.generated.h"

UCLASS()
class PROCEDURALGENERATOR_API AProceduralGenerationActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralGenerationActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


//protected:
	/**
	*  Create/replace a section for this procedural mesh component.
	*  @param  SectionIndex        Index of the section to create or replace.
	*  @param  Vertices            Vertex buffer of all vertex positions to use for this mesh section.
	*  @param  Triangles           Index buffer indicating which vertices make up each triangle. Length must be a multiple of 3.
	*  @param  Normals             Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
	*  @param  UV0                 Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
	*  @param  VertexColors        Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
	*  @param  Tangents            Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
	*  @param  bCreateCollision    Indicates whether collision should be created for this section. This adds significant cost.
	*/
//	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh", meta = (AutoCreateRefTerm = "Normals,UV0,VertexColors,Tangents"))
	//	void CreateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FColor>& VertexColors, const TArray<FProcMeshTangent>& Tangents, bool bCreateCollision);

	/**
	*  Updates a section of this procedural mesh component. This is faster than CreateMeshSection, but does not let you change topology. Collision info is also updated.
	*  @param  Vertices            Vertex buffer of all vertex positions to use for this mesh section.
	*  @param  Normals             Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
	*  @param  UV0                 Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
	*  @param  VertexColors        Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
	*  @param  Tangents            Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
	*/
//	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh", meta = (AutoCreateRefTerm = "Normals,UV0,VertexColors,Tangents"))
	//	void UpdateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FColor>& VertexColors, const TArray<FProcMeshTangent>& Tangents);

	/** Clear a section of the procedural mesh. Other sections do not change index. */
//	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
//		void ClearMeshSection(int32 SectionIndex);

	
	
};
