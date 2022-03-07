// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <SteerProject/Node.h>

#include "SceneBuilder.generated.h"

UCLASS()
class STEERPROJECT_API ASceneBuilder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASceneBuilder();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = PathFinding)
	UTexture2D* MyTexture2D;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PathFinding)
	TSubclassOf<class AWall> WallClass;

	TArray<TArray<Node>> Map;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	TArray<TArray<Node>> GetMap() ;
};
