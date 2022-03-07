// Fill out your copyright notice in the Description page of Project Settings.

#include "Wall.h"

#include "SceneBuilder.h"

// Sets default values
ASceneBuilder::ASceneBuilder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASceneBuilder::BeginPlay()
{
	Super::BeginPlay();
    if (MyTexture2D == NULL) {
        return;
    }
    
    const FColor* FormatedImageData = static_cast<const FColor*>(MyTexture2D->PlatformData->Mips[0].BulkData.LockReadOnly());

    for (int32 X = 0; X < 32; X++)
    {
        TArray<Node> Line;
        for (int32 Y = 0; Y < 32; Y++)
        {
            FColor PixelColor = FormatedImageData[Y * 32 + X];

            if (PixelColor.R < 255) {
                GetWorld()->SpawnActor<AWall>(WallClass, FTransform(FRotator(0.f,0.f,0.f), FVector(GetActorLocation().X+X * 100.f, GetActorLocation().Y+ Y * 100.f,225.f), FVector(0.05f, 0.05f, 0.05f)));
                Line.Add(Node(X, Y, 0, 0, false));

            }
            else {
                Line.Add(Node(X, Y, 0, 0, true));

            }
        }
        Map.Add(Line);
    }

    MyTexture2D->PlatformData->Mips[0].BulkData.Unlock();
}

// Called every frame
void ASceneBuilder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<TArray<Node>> ASceneBuilder::GetMap()
{
    return  Map ;
}

